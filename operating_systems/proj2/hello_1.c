#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/sched.h>
#include <linux/proc_fs.h>
#define DEVICE_NAME "process_list"
#define CLASS_NAME "ps_list"
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lovelin Edward");
MODULE_DESCRIPTION("This module creates a /dev/process_list");

static int majorNumber;
static struct class* pslistClass = NULL;
static struct device* pslistDevice = NULL;
static char message[18] = "Hello from kernel";

static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);

struct task_struct *task_list;

static struct file_operations fops ={
  .open = dev_open,
  .read = dev_read,
  .release = dev_release
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
  
  printk(KERN_INFO "PSLISTChar: device class registered correctly\n");
  return 0;
}

static void __exit pslist_exit(void){
  device_destroy(pslistClass, MKDEV(majorNumber, 0));
  class_destroy(pslistClass);
  unregister_chrdev(majorNumber, DEVICE_NAME);
  printk(KERN_INFO "PSLISTChar: Goodbye from the PSLISTChar module!\n");
}

static int dev_open(struct inode *inodep, struct file *filep){
  printk(KERN_INFO "PSLISTChar: Device has been opened\n");
  return 0;
}

static int dev_release(struct inode *inodep, struct file *filep){
  printk(KERN_INFO "PSLISTChar: Device has been released\n");
  return 0;
}
static ssize_t dev_read(struct file * filep, char * buffer, size_t len, loff_t * offset){
  int error;
  rcu_read_lock();
    for_each_process(task_list){
      printk(KERN_INFO "PID=%d PPID=%d CPU=%d \n", task_list->pid, task_list->real_parent->pid, task_cpu(task_list));
    }
  rcu_read_unlock();
  error = copy_to_user(buffer, message, 18);
  if(error == 0){
    printk(KERN_INFO "PSLISTChar: Sent 18 characters to user space successfully");
    return 0;
  }else{
    printk(KERN_INFO "PSLISTChar: Failed to send characters to user space");
    return -EFAULT;
  }
}

module_init(pslist_init);
module_exit(pslist_exit);



