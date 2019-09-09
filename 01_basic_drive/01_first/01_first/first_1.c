/*test of param transmit form shell*/
/*add part
* linux/stat.h: the file decribe  permission of transmit user  
* linux/moduleparam.h: inlcude defien of module_param
* module_param(variable,variable type,permission) 
*/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/stat.h>
#include <linux/moduleparam.h>

/*deflaut value of whom and howmany*/
static char *whom ="world";
static int  howmany =1;
/*define howmany and whom can be valued of param from shell*/
module_param(howmany,int,S_IRUGO);
module_param(whom,charp,S_IRUGO);


static int __init first_init(void)
{
  printk(KERN_ALERT"this my first drive!\n");
  printk(KERN_ALERT"whom:%s,howmany: %d\n",whom,howmany);
  while(howmany--)
	printk(KERN_ALERT"whom:%s",whom);
  return 0;
}



static void  __exit first_exit(void)
{ 
  printk(KERN_ALERT"gye my first drive!\n");
}

module_init(first_init);
module_exit(first_exit);
MODULE_LICENSE("GPL");

