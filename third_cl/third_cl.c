#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <asm/uaccess.h>
#include <asm/signal.h>
#include <asm/io.h>
#include <asm/irq.h>

#define LED_ON(x)  (*gpfdat &= ~(1<<x))
#define LED_OFF(x)  (*gpfdat |= (1<<x))

static struct class *third_class;
static struct class_device *third_drv;

volatile unsigned long *gpfcon;
volatile unsigned long *gpgcon;

volatile unsigned long *gpfdat;
volatile unsigned long *gpgdat;


wait_queue_head_t button_waitq;
int major,ev_press;
char val;
char LED_state[3];

static irqreturn_t third_cl (int irq, void * dev)
{
	printk("this is hanled programer!!!\n");
	val = *gpfdat;
	if(!(val & (1<<0)))	 
	{ 
		if(LED_state[0]==0) 
			{ LED_ON(4);LED_state[0] = 1;}
		else 	
			{ LED_OFF(4);LED_state[0] = 0;}
	}
	
	if(!(val & (1<<2)))
	{  
		if(LED_state[1] == 0) 
			{ LED_ON(5);LED_state[1] = 1;}
		else 	
			{ LED_OFF(5);LED_state[1] = 0;}
	}
	
	val=*gpgdat;
	if(!(val & (1<<3)))	
	{
		if(LED_state[2] == 0)
			{ LED_ON(6);LED_state[2] = 1;}
		else 
			{ LED_OFF(6);LED_state[2] = 0;}
	}

	
    ev_press = 1;                  
    wake_up_interruptible(&button_waitq); 
	
	return IRQ_RETVAL(IRQ_HANDLED);
}

static int third_cl_open(struct inode *inode,struct file *file)
{
	/*-key and led  io init
	  -led:gpf4\5\6 key：gpf 0\2 gpg 3 */
	  
	*gpfcon &= ~ ((0x3 << 4) | (0x3 << 5) | (0x3 << 6));
	*gpfcon |= ((0x1 << 4) | (0x1 << 5) | (0x1 << 6));

	
	/*IRQ _REQUEST*/
	request_irq(IRQ_EINT0, third_cl, IRQF_TRIGGER_FALLING, "cl_1", 1);
	request_irq(IRQ_EINT2, third_cl, IRQF_TRIGGER_FALLING, "cl_2", 1);
	request_irq(IRQ_EINT11, third_cl, IRQF_TRIGGER_FALLING, "cl_3", 1);
	
	return 0;
}

static void third_cl_release(struct inode *inode,struct file *file)
{
	free_irq(IRQ_EINT0, 1);
	free_irq(IRQ_EINT2, 1);
	free_irq(IRQ_EINT11, 1);
}

static ssize_t third_cl_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	return 0;
}

static ssize_t third_cl_read(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	
	printk("this is drive_read!\n");
	wait_event_interruptible(button_waitq, ev_press);

	copy_to_user(buf, &LED_state, 3);
	ev_press = 0;
	
	return 3;
}

static struct file_operations third_cl_fop = {
	.owner = THIS_MODULE,
	.open  = third_cl_open,
	.release = third_cl_release,
	.write = third_cl_write,
	.read  = third_cl_read,
};


static int __init third_init(void){
	major = register_chrdev(0,"third_cl",&third_cl_fop);
	third_class = class_create(THIS_MODULE,"third_cl");
	third_drv = class_device_create(third_class,NULL,MKDEV(major,0),NULL,"third_key");

	init_waitqueue_head(&button_waitq);
	gpfcon = (volatile unsigned long *)ioremap(0x56000050,16);
	gpfdat = gpfcon +1;

	gpgcon = (volatile unsigned long *)ioremap(0x56000060,16);
	gpgdat = gpgcon +1;
	return 0;

} 

static void __exit third_exit(void){
	unregister_chrdev(major,"third_cl");
	class_device_unregister(third_drv);
	class_destroy(third_class);
	iounmap(gpfcon);
	iounmap(gpgcon);	
} 

module_init(third_init);
module_exit(third_exit);
MODULE_LICENSE("GPL");