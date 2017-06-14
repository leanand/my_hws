#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>

static int input_pid = 0;
module_param(input_pid, int, 0);

static long j_handle_mm_fault(struct vm_area_struct *vma, unsigned long address, unsigned int flags)
{
  if(current->pid == input_pid){
    printk(KERN_INFO "%lu\t%d",address, input_pid);
  }
  jprobe_return();
  return 0;
}

static struct jprobe my_jprobe = {
  .entry      = j_handle_mm_fault,
  .kp = {
    .symbol_name  = "handle_mm_fault",
  },
};

static int __init jprobe_init(void)
{
  int ret;
  ret = register_jprobe(&my_jprobe);
  if (ret < 0) {
    printk(KERN_INFO "register_jprobe failed, returned %d\n", ret);
    return -1;
  }
  printk(KERN_INFO "Planted jprobe at %p, handler addr %p\n", my_jprobe.kp.addr, my_jprobe.entry);
  return 0;
}

static void __exit jprobe_exit(void)
{
  unregister_jprobe(&my_jprobe);
  printk(KERN_INFO "jprobe at %p unregistered\n", my_jprobe.kp.addr);
}

module_init(jprobe_init)
module_exit(jprobe_exit)
MODULE_LICENSE("GPL");