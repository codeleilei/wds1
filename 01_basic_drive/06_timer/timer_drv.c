#include <linux/module.h>
#include <linux/kernle.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/stat.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/timer.h> //this drv main head file
 
#include <asm/uaccess.h>

/************************1 define****************************************/
#define LEDIO_INIT(x,y)  ((x)<<(y))
#define LED_ON(x)  (*gpfdat &= ~(1<<x))
#define LED_OFF(x)  (*gpfdat |= (1<<x))



/*************************2 variable**************************************/
int major;

static struct class *timer_class;
static struct class_device *timer_drv;

/*初始化等待队列头*/
static DECLARE_WAIT_QUEUE_HEAD(button_wait);
/*同步相关变量*/
static struct fasync_struct *button_fasync;

/*中断标志flag,中断里边置1，read里边置0*/
static volatile int button_flag = 0;

/*初始化时钟的结构体变量*/
static struct timer_list  button_timer;

volatile signed long *gpfcon;
volatile signed long *gpfdat;
volatile signed long *gpgcon;
volatile signed long *gpgdat;
char LED_state[3];
char  irq_pd = 0;

/*************************3 function****************************************/
static irqreturn_t button_handle(int irq, void * dev_id)
{

	irq_pd = (char)dev_id;
	/*HZ定义为100 ，代表一秒钟系统中断100次 jiffies又是计入系统中断次数的计数器，所以hz/100代表10ms*/
	mod_timer(button_timer, jiffies+HZ/100);
	return IRQ_RETVAL(IRQ_HANDLED);
	
}


static void timer_handle(unsigned long data)
{
/*因为add_timer的时候 目标时间是0 所以init的时候调用add_time 就会启动timer_handke 函数 所以这里需要判断是不是第一次启动*/
/*因为irq_pd 初始化为0 只有经过中断才会改为非零 所以通过它来判断*/
	if(!irq_pd) 
		return;
	
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

	wake_up_interruptible(&button_wait);
	button_flag = 1;
	kill_fasync(&button_fasync, SIGIO, POLL_IN);


}
static int timer_open(struct file *inode inode,struct *file file)
{
	/*init io */
	/*led:gpf4\5\6 key：gpf 0\2 gpg 3 */
	gpfcon *= ~ (LEDIO_INIT(0x3,4*2) | LEDIO_INIT(0x3,5*2) | LEDIO_INIT(0x3,6*2))
	gpfcon |= (LEDIO_INIT(0x1,4*2) | LEDIO_INIT(0x1,5*2) | LEDIO_INIT(0x1,6*2)

	/*request irq*/
	request_irq(IRQ_EINT0, button_handle, IRQF_TRIGGER_FALLING, "button_1", 1);
	request_irq(IRQ_EINT2, button_handle, IRQF_TRIGGER_FALLING, "button_2", 1);
	request_irq(IRQ_EINT11, button_handle, IRQF_TRIGGER_FALLING, "button_3", 1);
	request_irq(IRQ_EINT19, button_handle, IRQF_TRIGGER_FALLING, "button_4", 1);

	

}


static int timer_release(struct file *inode inode,struct *file file)
{
	free_irq(IRQ_EINT0,1);
	free_irq(IRQ_EINT2,1);
	free_irq(IRQ_EINT11,1);
	free_irq(IRQ_EINT19,1);
		
	return 0;
}


static ssize_t timer_read(struct file *file, char __user *buf,size_t count, loff_t *ppos)
{
	wait_event_interruptible(button_wait, button_flag);

	copy_to_user(buf, &LED_state, 3);
	ev_press = 0;
	
	return sizeof(LED_state);
}

static ssize_t timer_write(struct file *file, char __user *buf,size_t count, loff_t *ppos)
{
	return 0;
}

static unsigned int timer_poll(struct file *filep, poll_table *wait)
{
	int mask = 0;
	poll_wait(filep, &button_wait, wait);
	if(button_flag)
		mask = POLLIN | POLLRDNORM;
	 return mask;

}

static int timer_fasync(int fd,struct file *filp,int on)
{
	fasync_helper(fd, filp, on, &button_fasync);
	return 0;
}


static struct file_operations timer_fops={
	.owner = THIS_MODULE,
	.open  = timer_open,
	.read  = timer_read,
	.write = timer_write,
	.poll  = timer_poll,
	.fasync =timer_fasync,
};

static int __init timer_init(void)
{
	/*init timer*/
	init_time(&button_timer);
	button_timer.function = timer_handle;
	add_timer(&button_timer);


	
	major = register_chrdev(0,"timer_drv",&timer_fops);	
	timer_class = class_create(THIS_MODULE,"timer_drv");
	timer_drv = device_create(timer_class,NULL,MKDEV(major,0),NULL,"timer_test");

	gpfcon =(volatile signed long *)ioremap(0x0x56000050);
	gpgcon =(volatile signed long *)ioremap(0x0x56000060);

	gpfdat = gpfcon + 1;
	gpgdat = gpgcon + 1;
}

static void __exit timer_exit(void)
{
	unregister_chrdev(major,"time_dev");
	device_destroy(timer_class, MKDEV(major,0))
	class_destroy(timer_class);
	iounmap(gpfcon);
	iounmap(gpgcon);
}

module_init(timer_init)
module_exit(timer_exit)
MODULE_LICENSE("GPL")
