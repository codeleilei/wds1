#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/irq.h>
#include <linux/fs.h>


#include <asm/access.h>


struct input_dev *button_dev;
struct timer_list button_timer;



static void timer_handle(unsigned long data)
{
	



}


static void __init  input_init()
{
	int i;
	button_dev = input_allocate_device();
	/*设置 事件*/
	/*设置那一列事件*/
	set_bit(EV_KEY,button_dev->evbit);
	set_bit(EV_REP,button_dev->evbit);

	/*设置这个类会发生什么事件*/
	set_bit(KEY_L,button_dev->keybit);	
	set_bit(KEY_S,button_dev->keybit);
	set_bit(KEY_ENTER,button_dev->keybit);
	set_bit(KEY_LEFTSHIFT,button_dev->keybit);


	time_init(&button_timer);
	button_timer->function = timer_handle;
	add_timer(&button_timer);

	for(i=0;i<4;i++){
		request_irq(pin[i].irq, buttonirq_handle, , const char * name, void * dev)
	}

}



static void __exit   input_exit()
{



}

