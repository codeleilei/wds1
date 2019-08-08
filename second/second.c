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

	/*
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/stat.h>
#include <linux/types.h>
#include <linux/module.h>
#include <asm/uaccess.h>
#include <asm/io.h>
*/
struct led{
	char num;
	char state;
};


static struct class *sec_class;
static struct class_device *sec_cdev;

volatile unsigned long *gpfcon;
volatile unsigned long *gpgcon;

volatile unsigned long *gpfdat;
volatile unsigned long *gpgdat;



static int second_open(struct inode *inode,struct file *file)
{
	/*key and led  io init*/
	/*led:gpf4\5\6 key：gpf 0\2 gpg 3*/ 
	*gpfcon &= ~ ((0x3 << 4) | (0x3 << 5) | (0x3 << 6) | (0x3 << 0) | (0x3 << 2));
	*gpgcon &= ~ (0x3 << 3);

	*gpfcon |= ((0x1 << 4) | (0x1 << 5) | (0x1 << 6));

	return 0;
}

static ssize_t second_read(struct file *file,char __user *buf,size_t count,loff_t *ppos)
{
	unsigned char key_val[3];
	int regval;
	
	if(count != sizeof(key_val))
		return -1;
	regval = *gpfdat;
	
	key_val[0] = (*gpfdat & (1<<0))? 1 : 0;
	key_val[1] = (*gpfdat & (1<<2))? 1 : 0;
	key_val[2] = (*gpgdat & (1<<3))? 1 : 0;

	copy_to_user(buf,key_val,sizeof(key_val));
	
	return sizeof(key_val);
	
}

static ssize_t second_write(struct file *file,char __user *buf,size_t count,loff_t *ppos)
{
//	struct led led_s;
//	copy_from_user(&led_s,buf,count);
	int val;
	copy_from_user(&val,buf,count);
	printk("val/10=%d,val%10=%d,\n",val/10,val%10);
	switch(val/10){
	case 1:
			if(val%10 == 0)
				*gpfdat &= ~(1<<4);
			else
				*gpfdat |= (1<<4);
			break;
	case 2:
			if(val%10 == 0)
				*gpfdat &= ~(1<<5);
			else
				*gpfdat |= (1<<5);
			break;

	case 3:
			if(val%10 == 0)
				*gpfdat &= ~(1<<6);
			else
				*gpfdat |= (1<<6);
			break;

	default:
			*gpfdat |= ((1<<6) | (1<<5) | (1<<4));  // all down
	}

	return sizeof(val);

}


static struct file_operations second_fops={
	.owner = THIS_MODULE,
	.open  = second_open,
	.write = second_write,
	.read  = second_read,
};


int major;

static int __init second_init(void)
{
	major = register_chrdev(0,"sec_dev",&second_fops);
	sec_class = class_create(THIS_MODULE,"sec_dev");
	sec_cdev = class_device_create(sec_class,NULL,MKDEV(major,0),NULL,"cl_sec");
	
	gpfcon = (volatile unsigned long *)ioremap(0x56000050,16);
	gpfdat = gpfcon +1;

	gpgcon = (volatile unsigned long *)ioremap(0x56000060,16);
	gpgdat = gpgcon +1;
	return 0;
}

static void __exit second_exit(void)
{
	unregister_chrdev(major,"sec_dev");
	class_device_unregister(sec_cdev);
	class_destroy(sec_class);
	iounmap(gpfcon);
	iounmap(gpgcon);
}


module_init(second_init);
module_exit(second_exit);
MODULE_LICENSE("GPL");

