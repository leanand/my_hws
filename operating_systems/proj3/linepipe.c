#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/sched.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/delay.h>
#define DEVICE_NAME "linepipe"
#define CLASS_NAME "linepipe"
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lovelin Edward");
MODULE_DESCRIPTION("This module creates a /dev/linepipe");

static int majorNumber;
static struct class* pslistClass = NULL;
static struct device* pslistDevice = NULL;

static int max_buffer_size = 5;
static int N = 5;
module_param(N, int, 0);

static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset);

wait_queue_head_t buffer_empty_queue, buffer_full_queue;
struct buffer_struct {
  char * line;
  struct buffer_struct * next;
};

int list_size;
struct buffer_struct * buffer_list;
static DEFINE_MUTEX(buffer_mutex_lock);

static struct file_operations fops ={
  .owner = THIS_MODULE,
  .open = dev_open,
  .read = dev_read,
  .write = dev_write,
  .release = dev_release
};

static int __init linepipe_init(void){
  printk(KERN_INFO "LINEPIPEChar: Initializing the LINEPIPEChar module\n");
  majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
  if(majorNumber < 0){
    printk(KERN_ALERT "LINEPIPEChar: failed to register a major number\n"); 
    return majorNumber;   
  }

  pslistClass = class_create(THIS_MODULE, CLASS_NAME);
  if(IS_ERR(pslistClass)){
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_ALERT "LINEPIPEChar: failed to create a device class\n"); 
    return PTR_ERR(pslistClass);
  }

  printk(KERN_INFO "LINEPIPEChar: device class registered correctly\n");

  pslistDevice = device_create(pslistClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
  if(IS_ERR(pslistDevice)){
    class_destroy(pslistClass);
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_ALERT "LINEPIPEChar: failed to create the device\n"); 
    return PTR_ERR(pslistDevice);
  }
  printk(KERN_INFO "LINEPIPEChar: device  created correctly\n");

  buffer_list = NULL;
  list_size = 0;
  init_waitqueue_head(&buffer_empty_queue);
  init_waitqueue_head(&buffer_full_queue);
  max_buffer_size = N;
  printk(KERN_INFO "LINEPIPEChar: Buffer size is %d", max_buffer_size);
  return 0;
}

static void __exit linepipe_exit(void){
  struct buffer_struct * current_buff, *target_buff;
  current_buff = NULL;
  while(current_buff != NULL){
    target_buff = current_buff;
    current_buff  = target_buff->next;
    kfree(target_buff->line);
    kfree(target_buff);
  }
  device_destroy(pslistClass, MKDEV(majorNumber, 0));
  class_destroy(pslistClass);
  unregister_chrdev(majorNumber, DEVICE_NAME);
  printk(KERN_INFO "LINEPIPEChar: Goodbye from the LINEPIPEChar module!\n");
}

static int dev_open(struct inode *inodep, struct file *filep){
  printk(KERN_INFO "LINEPIPEChar: Device has been opened\n");
  return 0;
}

static int dev_release(struct inode *inodep, struct file *filep){
  printk(KERN_INFO "LINEPIPEChar: Device has been released\n");
  return 0;
}
static ssize_t dev_read(struct file * filep, char * buffer, size_t len, loff_t * offset){
  struct buffer_struct * current_buff;
  int str_len = 0;
  if(mutex_lock_interruptible(&buffer_mutex_lock) < 0){
    return -ERESTARTSYS;
  }
  if(list_size == 0){
    mutex_unlock(&buffer_mutex_lock);
    printk(KERN_INFO "LINEPIPEChar:Read: Process Blocked");
    if(wait_event_interruptible(buffer_empty_queue, list_size > 0) < 0){
      printk(KERN_INFO "LINEPIPEChar:  Read wait interrupted");  
      return -ERESTARTSYS;
    }
    printk(KERN_INFO "LINEPIPEChar:Read: Process Unblocked");
    if(mutex_lock_interruptible(&buffer_mutex_lock) < 0){
      return -ERESTARTSYS;
    }
  }
  current_buff  = buffer_list;
  str_len = strlen(current_buff->line) + 1;
  if(copy_to_user(buffer, current_buff->line, str_len ) != 0 ){
    printk(KERN_INFO "LINEPIPEChar:  Error in copying buffer to user");
    return 0;
  }
  buffer_list = current_buff->next;
  kfree(current_buff->line);
  kfree(current_buff);
  list_size --;
  if(list_size == max_buffer_size - 1){
    wake_up_interruptible(&buffer_full_queue);
  }
  mutex_unlock(&buffer_mutex_lock);
  printk(KERN_INFO "LINEPIPEChar: %d bytes read", str_len);
  return str_len;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
  struct buffer_struct * current_buff, * temp_buff;
  if(mutex_lock_interruptible(&buffer_mutex_lock) < 0){
    return -ERESTARTSYS;
  }
  if( list_size == max_buffer_size){
    printk(KERN_INFO "LINEPIPEChar: Cannot write, buffer is full, making the process wait");
    mutex_unlock(&buffer_mutex_lock);
    if(wait_event_interruptible(buffer_full_queue, list_size < max_buffer_size) < 0){
      printk(KERN_INFO "LINEPIPEChar: Write : Wait interruption");
      return -ERESTARTSYS;
    }
    printk(KERN_INFO "LINEPIPEChar:Write: Process Unblocked");
    if(mutex_lock_interruptible(&buffer_mutex_lock) < 0){
      return -ERESTARTSYS;
    }
  }
  if((current_buff = kmalloc(sizeof(struct  buffer_struct), GFP_KERNEL)) == NULL){
    mutex_unlock(&buffer_mutex_lock);
    printk(KERN_INFO "LINEPIPEChar: Error in allocating memory in buffer");
    return -1;
  }
  current_buff->next = NULL;
  if((current_buff->line = kmalloc(sizeof(char) * (len + 1), GFP_KERNEL)) == NULL){
    mutex_unlock(&buffer_mutex_lock);
    printk(KERN_INFO "LINEPIPEChar: Error in allocating memory in buffer->line");
    return -1;
  }
  if(copy_from_user(current_buff->line, buffer, len) != 0){
    mutex_unlock(&buffer_mutex_lock);
    printk(KERN_INFO "LINEPIPEChar: Error in copying buffer from user");
    kfree(current_buff->line);
    kfree(current_buff);
    return -1;
  }
  current_buff->line[len] = '\0';
  if(list_size == 0){
    buffer_list = current_buff;
    list_size ++;
    wake_up_interruptible(&buffer_empty_queue);
  }else{
    temp_buff = buffer_list;
    while(temp_buff -> next != NULL){
      temp_buff = temp_buff->next;
    }
    temp_buff->next = current_buff;
    list_size ++;
  }
  mutex_unlock(&buffer_mutex_lock);
  printk(KERN_INFO "LINEPIPEChar: %ld bytes written", len);
  return len;
}

module_init(linepipe_init);
module_exit(linepipe_exit);



