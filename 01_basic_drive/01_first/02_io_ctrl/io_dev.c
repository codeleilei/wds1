/*02_ioctrl text*/
/*goal:to ctrl 2440 led!
*main api: ioremap
*/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/stat.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/types.h>  //一些描述符
#include <linux/fs.h> //包含了file结构
#include <asm/uaccess.h> //copy_to ...
#include <asm/io.h>  //ioremap




volatile unsigned long * gpfcon=NULL;
volatile unsigned long * gpfdat=NULL;

static struct class *io_class;
static struct class_device *io_dev;

static int io_open(struct inode *inode,struct file *filp)
{
	/*Initial GPF*/
	*gpfcon &= ~((0x3<<(4*2)) | (0x3<<(5*2)) | (0x3<<(6*2)));//目标为清零
	*gpfcon	|= ((0x1<<(4*2)) | (0x1<<(5*2)) | (0x1<<(6*2)));

	return 0;
}

static int io_release(struct inode *inode,struct file *filp)
{
	
	return 0;
}

static ssize_t io_read(struct file *file,const char __user *buf,size_t count,loff_t *ppos)
{
	return 0;
}

static ssize_t io_write(struct file *file,const char __user *buf,size_t count,loff_t *ppos)
{
	int val;
	copy_from_user(&val,buf,count);

	switch(val){
	case 1:
			*gpfdat &= ~(1<<4);
			break;
	case 2:
			*gpfdat &= ~(1<<5);
			break;
	case 3:
			*gpfdat &= ~(1<<6);
			break;
	case 4:
			*gpfdat &= ~((1<<6) | (1<<5) | (1<<4));  //all up
			break;
	case 5:
			*gpfdat |= ((1<<6) | (1<<5) | (1<<4));  // all down
			break;
	default:
			*gpfdat |= ((1<<6) | (1<<5) | (1<<4));  // all down
	}

	return 0;
}
/*
static int io_ioctl(struct inode *inode,struct file *file,unsigned int,unsigned long)
{

	return 0;
}
*/

struct file_operations io_fops={
	.owner = THIS_MODULE,
	.read  = io_read,
	.write = io_write,
	.open  = io_open,
	.release = io_release,
	//.ioctl = io_ioctl,

};
	
int major;
static int __init first_init(void)
{
	major = register_chrdev(0,"io_text",&io_fops);
	io_class = class_create(THIS_MODULE,"io_text");
	io_dev =  class_device_create(io_class,NULL,MKDEV(major,0),NULL,"io_ctl");

	gpfcon = (volatile unsigned long *)ioremap(0x56000050,16);
	gpfdat = gpfcon + 1;
 	return 0;
}



static void  __exit first_exit(void)
{ 
	unregister_chrdev(major,"io_text");
	class_device_unregister(io_dev);
	class_destroy(io_class);
	iounmap(gpfcon);
}

module_init(first_init);
module_exit(first_exit);
MODULE_LICENSE("GPL");

