#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/semaphore.h>
#include<asm/uaccess.h>
#include<linux/sched.h>


//virtual dev
struct virtual_device
{
        char *data;
        struct semaphore sem;

}virtual_device;

// cracter device def
struct cdev *mcdev;
int major_number;
int ret;
dev_t dev_num;
struct task_struct *task_list;
#define DEVICE_NAME     "process_list"

int device_open(struct inode *inode, struct file *filp)
{
        if(down_interruptible(&virtual_device.sem) !=0)
        {
                printk(KERN_ALERT "couldn't lock dev when open");
                return -1;
        }
	printk(KERN_INFO "opened dev");
        return 0;
}

static ssize_t device_read(struct file* filp, char* bufStoreData, size_t bufCount, loff_t* curOffset)

{
	
//      char *bufStoreData;
        char *proc_id,*status;
        for_each_process(task_list)
        {
                proc_id=(task_list->pid);
                status=(task_thread_info(task_list)->cpu);
                virtual_device.data=proc_id;//strcat("pid:",proc_id);
                bufStoreData= strcat(bufStoreData,virtual_device.data);

        }
        copy_to_user(bufStoreData,virtual_device.data,bufCount);
//      }
        return 0;
}



int device_close(struct inode *inode,struct file *filp)
{
	up(&virtual_device.sem);
        printk(KERN_INFO "closed device");
        return 0;
}


struct file_operations fops =
{
        .owner = THIS_MODULE,
        .open=device_open,
        .release = device_close,
  //      .write = device_write,
        .read= device_read

};

static int driver_entry(void)
{       //fake =(char) malloc(sizeof(char));
        ret=alloc_chrdev_region(&dev_num,0,1,DEVICE_NAME);
        if(ret<0)
        {
                printk(KERN_ALERT "failed to alloc majornum");
                return ret;
        }
        major_number = MAJOR(dev_num);
        printk(KERN_INFO "major num is : %d:",major_number);
        printk(KERN_INFO "\tuse \"mknod /dev/%s c %d 0\" for device file",DEVICE_NAME,major_number);   //to find in dmesg
        mcdev=cdev_alloc();
        mcdev->ops = &fops;
        mcdev->owner = THIS_MODULE;
        ret = cdev_add(mcdev,dev_num,1);
        if(ret<0)
        {
                printk(KERN_ALERT "unable to return to kernel");
                return ret;
        }
        sema_init(&virtual_device.sem,1);
        return 0;
}

static void driver_exit(void)
{
        cdev_del(mcdev);
        unregister_chrdev_region(dev_num,1);
        printk(KERN_ALERT "unloaded module");
}


module_init(driver_entry);
module_exit(driver_exit);
