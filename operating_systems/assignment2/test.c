//Kernel module

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

#define  DEVICE_NAME "pipe"    
#define  CLASS_NAME  "ch"    
#define BUFFER_LINE_SIZE 110

MODULE_LICENSE("GPL");
MODULE_AUTHOR("HARINI");
MODULE_DESCRIPTION("A simple character device driver");  
        

static int majorNumber;  
static struct device* deviceName = NULL;
static struct class*  className  = NULL;  
static char ** buffer_table;
static int max_table_size = 5;
module_param(max_table_size, int, 0);
static int read_ptr = -1;
static int write_ptr = -1;
static int elements_count = 0;
wait_queue_head_t empty_buffer_queue;
wait_queue_head_t full_buffer_queue;
static DEFINE_MUTEX(buffer_lock);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops =
{
   .open = device_open,
   .read = device_read,
   .write = device_write,
   .release = device_release
};
 
static int initialize_device(void)
{
   int i ;
   printk(KERN_INFO "Initiating the Character device");
   majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
   if (majorNumber<0){
      printk(KERN_ALERT "Failed to register Majornumber \n");
      return majorNumber;
   }
   printk(KERN_INFO "Device loaded successfully\n");
    
   className = class_create(THIS_MODULE, CLASS_NAME);
   if (IS_ERR(className)){             
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to register device class\n");
      return PTR_ERR(className);          
   }
   printk(KERN_INFO "Class register successfully\n");
    
   deviceName = device_create(className, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
   if (IS_ERR(deviceName)){  
                 
      class_destroy(className);           
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to create the device\n");
      return PTR_ERR(deviceName);
   }
   printk(KERN_INFO "Device class created \n"); 

   buffer_table = kcalloc(max_table_size , sizeof(char *), GFP_KERNEL);
   for(i = 0; i < max_table_size ; i ++)
   {
      buffer_table[i] = kcalloc(BUFFER_LINE_SIZE , sizeof(char), GFP_KERNEL);
   }
   init_waitqueue_head(&empty_buffer_queue);
   init_waitqueue_head(&full_buffer_queue);
   return 0;
}

static void exit_device(void)
{
   int i;
   for(i = 0; i < max_table_size; i ++)
   {
      kfree(buffer_table[i]);
   }
   kfree(buffer_table);
   device_destroy(className, MKDEV(majorNumber, 0));     
   class_unregister(className);                          
   class_destroy(className);                             
   unregister_chrdev(majorNumber, DEVICE_NAME);             
   printk(KERN_INFO "Exit from the module\n");
}

static int device_open(struct inode *inodep, struct file *filep)
{
   printk(KERN_INFO "Device has been opened\n");
   return 0;
}
 
static ssize_t device_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{	
   char * target_str;
   int str_len;
   if(mutex_lock_interruptible(&buffer_lock) < 0)
   {
      return -ERESTARTSYS;
   }
   if(elements_count == 0 )
   {
      mutex_unlock(&buffer_lock);
      if(wait_event_interruptible(empty_buffer_queue, elements_count > 0) < 0){
         return -ERESTARTSYS;
      }
      if(mutex_lock_interruptible(&buffer_lock) < 0){
         return -ERESTARTSYS;
      }
   }
   printk(KERN_INFO "before read the read ptr %d ==> write_ptr %d", read_ptr, write_ptr);
   target_str = buffer_table[read_ptr];
   printk(KERN_INFO "The address is ====> %p", target_str);
   str_len = strlen(target_str) + 1;
   if(copy_to_user(buffer, target_str, str_len ) != 0 ){
      printk(KERN_INFO "Cannot copy to user");
      return -1;
   }
   target_str[0] = '\0';
   if(read_ptr == write_ptr)
   {
      read_ptr = -1;
      write_ptr = -1;
   }else if(read_ptr == max_table_size - 1)
   {
      read_ptr = 0;
   }else
   {
      read_ptr ++;
   }
   elements_count --;
   if(elements_count == max_table_size - 1)
   {
      wake_up_interruptible(&full_buffer_queue);
   }
   printk(KERN_INFO "after read the read ptr %d ==> write_ptr %d", read_ptr, write_ptr);
   mutex_unlock(&buffer_lock);
   return str_len;
}

static ssize_t device_write(struct file * filep, const char * buffer, size_t len, loff_t *offset)
{
   char * target_str;
   if(mutex_lock_interruptible(&buffer_lock) < 0){
      return -ERESTARTSYS;
   }
   if(elements_count == max_table_size)
   {
      mutex_unlock(&buffer_lock);
      if(wait_event_interruptible(full_buffer_queue, elements_count < max_table_size) < 0)
      {
         return -ERESTARTSYS;
      }
      if(mutex_lock_interruptible(&buffer_lock) < 0)
      {
         return -ERESTARTSYS;
      }
   }
   printk(KERN_INFO "before write the read ptr %d ==> write_ptr %d", read_ptr, write_ptr);
   if(read_ptr == -1)
   {
      write_ptr = 0;
      read_ptr = 0;
   }else if(write_ptr == max_table_size - 1 && read_ptr != 0)
   {
      write_ptr = 0;
   }else
   {
      write_ptr ++;
   }

   target_str = buffer_table[write_ptr];

   if(copy_from_user(target_str, buffer, len ) != 0 ){
      printk(KERN_INFO "Cannot copy from user");
      return -1;
   }
   target_str[len] = '\0';
   elements_count ++;
   if(elements_count == 1)
   {
      wake_up_interruptible(&empty_buffer_queue);
   }
   printk(KERN_INFO "after write the read ptr %d ==> write_ptr %d", read_ptr, write_ptr);
   mutex_unlock(&buffer_lock);
   return len;
}
 
static int device_release(struct inode *inodep, struct file *filep)
{
   printk(KERN_INFO "Device unloaded successfully \n");
   return 0;
}
 
module_init(initialize_device);
module_exit(exit_device);

