#include <linux/module.h>
#include <linux/init.h>

static int __init first_init(void)
{
  printk(KERN_ALERT"this my first drive!\n");
  return 0;
}



static void  __exit first_exit(void)
{
  printk(KERN_ALERT"gye my first drive!\n");
}

module_init(first_init);
module_exit(first_exit);
MODULE_LICENSE("GPL");

