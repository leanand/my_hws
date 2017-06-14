#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/sched.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>

#define DEVICE_NAME "process_list"
#define CLASS_NAME "ps_list"
#define MAX_PROCESS 300
#define PER_PROCESS_MAX_SIZE 21
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lovelin Edward");
MODULE_DESCRIPTION("This module creates a /dev/process_list");

static int majorNumber;
static struct class* pslistClass = NULL;
static struct device* pslistDevice = NULL;

static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);

struct task_struct *task_list;

static struct file_operations fops ={
  .owner = THIS_MODULE,
  .open = dev_open,
  .read = dev_read,
  .release = dev_release
};

struct ps_list {
  int size;
  char *buffer;
};

static int __init pslist_init(void){
  printk(KERN_INFO "PSLISTChar: Initializing the PSLISTChar module\n");
  majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
  if(majorNumber < 0){
    printk(KERN_ALERT "PSLISTChar: failed to register a major number\n"); 
    return majorNumber;   
  }

  pslistClass = class_create(THIS_MODULE, CLASS_NAME);
  if(IS_ERR(pslistClass)){
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_ALERT "PSLISTChar: failed to create a device class\n"); 
    return PTR_ERR(pslistClass);
  }

  printk(KERN_INFO "PSLISTChar: device class registered correctly\n");

  pslistDevice = device_create(pslistClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
  if(IS_ERR(pslistDevice)){
    class_destroy(pslistClass);
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_ALERT "PSLISTChar: failed to create the device\n"); 
    return PTR_ERR(pslistDevice);
  }
  
  printk(KERN_INFO "PSLISTChar: device  created correctly\n");
  return 0;
}

static void __exit pslist_exit(void){
  device_destroy(pslistClass, MKDEV(majorNumber, 0));
  class_destroy(pslistClass);
  unregister_chrdev(majorNumber, DEVICE_NAME);
  printk(KERN_INFO "PSLISTChar: Goodbye from the PSLISTChar module!\n");
}

static int dev_open(struct inode *inodep, struct file *filep){
  struct ps_list * ps_listp;
  int offset = 0;

  printk(KERN_INFO "PSLISTChar: Device has been opened\n");
  ps_listp = kmalloc(sizeof(struct ps_list) , GFP_USER);
  if( !ps_listp){
    printk(KERN_INFO "PSLISTChar: Error: Cannot allocate memory");
    return -ENOMEM;
  }
   ps_listp->size = 0;
   ps_listp->buffer = kmalloc(sizeof(char) * MAX_PROCESS * PER_PROCESS_MAX_SIZE, GFP_USER);
   if(!ps_listp->buffer){
      printk(KERN_INFO "PSLISTChar: Error: Cannot allocate memory");
      kfree(ps_listp);
      return -ENOMEM;  
   }
   filep->private_data = ps_listp;
   rcu_read_lock();
    for_each_process(task_list){
      // printk(KERN_INFO "%d %d", task_list->pid, task_list->real_parfent->pid)
      offset += sprintf(ps_listp->buffer + offset, "%d %d %d %ld\n", task_list->pid, task_list->real_parent->pid, task_cpu(task_list), task_list->state);
    }
    ps_listp->buffer[offset] = '\0';
    ps_listp->size = offset;
    printk(KERN_INFO "The size stored is %d", offset);
  rcu_read_unlock();
  return 0;
}

static int dev_release(struct inode *inodep, struct file *filep){
  struct ps_list * ps_listp = filep->private_data;
  kfree(ps_listp->buffer);
  kfree(ps_listp);
  printk(KERN_INFO "PSLISTChar: Device has been released\n");
  return 0;
}
static ssize_t dev_read(struct file * filep, char * buffer, size_t len, loff_t * offset){
  int error;
  size_t actual_length;
  size_t remain_length;
  char _buffer[len];
  struct ps_list * ps_listp = filep->private_data;
  if(*offset  >= ps_listp->size){
    return 0;
  }
  remain_length = ps_listp->size - *offset;
  actual_length = (remain_length >= len - 1 ) ? len - 1 : remain_length;
  strncpy(_buffer, ps_listp->buffer + *offset, actual_length);
  _buffer[ actual_length] = '\0';
  actual_length ++;
  error = copy_to_user(buffer, _buffer, actual_length);
  if(error == 0){
    *offset = *offset + actual_length - 1;
    printk(KERN_INFO "PSLISTChar: Sent %zu characters to user space successfully", actual_length - 1);
    return actual_length;
  }else{
    printk(KERN_INFO "PSLISTChar: Failed to send characters to user space");
    return -EFAULT;
  }
}

module_init(pslist_init);
module_exit(pslist_exit);



