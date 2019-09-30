/*
呼吸灯：注意在需要获取引脚值的 需要先设置为输入默认 才能获得引脚的值  否者得到的一直是0
原理： 利用内核定时器实现简单的呼吸灯效果
后续：1、可添加opne read等函数 可通过应用层来控制呼吸灯的呼吸频率 
	  2、可添加pwm 让呼吸灯过渡的缓慢一点	  

	  author：cl
	  data: 2019.9.30
*/
#include <linux/err.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h> 
#include <linux/gpio/consumer.h>
#include <linux/kernel.h>
#include <linux/leds.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/property.h>
#include <linux/slab.h>
#include <linux/workqueue.h>
#include <linux/timer.h>




struct gpio_led_data {
	struct device_node * breath_gpio;
	int gpio_cl2;
	int value;
	int flag;
	
};

static struct timer_list  breathled_timer;
static struct gpio_led_data * gpio_info;


static void timer_handle(unsigned long data)
{
	gpio_direction_input(gpio_info->gpio_cl2);

	gpio_get_value(gpio_info->gpio_cl2)? gpio_direction_output(gpio_info->gpio_cl2,0):\
											gpio_direction_output(gpio_info->gpio_cl2,1); 

	
	/*HZ定义为100 ，代表一秒钟系统中断100次 jiffies又是计入系统中断次数的计数器，所以hz/100代表10ms*/
	mod_timer(&breathled_timer, jiffies+HZ*2);
}

static int breath_led_probe(struct platform_device *pdev)
{

	gpio_info = devm_kzalloc(&pdev->dev,sizeof(struct gpio_led_data *), GFP_KERNEL); 
	
	if (!gpio_info) { 
      return -ENOMEM;
      }
	
   gpio_info->breath_gpio= pdev->dev.of_node; 
   gpio_info->gpio_cl2 = of_get_named_gpio(gpio_info->breath_gpio, "enable-gpios", 0); 
	printk("breath GPIO Test Program Probe\n"); 
	

	
	if (!gpio_is_valid(gpio_info->gpio_cl2)) {
    	printk("breath-gpio: %d is invalid\n", gpio_info->gpio_cl2); 
		return -ENODEV;
     } 
	if (gpio_request(gpio_info->gpio_cl2,"enable-gpios")) { 
        printk("gpio %d request failed!\n", gpio_info->gpio_cl2); 
        gpio_free(gpio_info->gpio_cl2); 
        return -ENODEV;
     } 

    int i=gpio_direction_output(gpio_info->gpio_cl2,1); 
	if(i){
		 printk("fail set gpio\n");  
		 return -1;
	}
	gpio_direction_input(gpio_info->gpio_cl2);
	printk("---------cl_probe-------value:%d---\n",gpio_get_value(gpio_info->gpio_cl2));
   
	i=gpio_direction_output(gpio_info->gpio_cl2,0); 
	if(i){
		 printk("fail set gpio\n");  
		 return -1;
	}
	//gpio_direction_input(gpio_info->gpio_cl2);

	printk("---------cl_probe22-------value:%d---\n",gpio_get_value(gpio_info->gpio_cl2));

	mod_timer(&breathled_timer, jiffies+HZ/100);

	return 0;
}

static int breath_led_remove(struct platform_device *pdev)
{
	gpio_free(gpio_info->gpio_cl2);
	return 0;
}

static const struct of_device_id of_gpio_leds_match[] = {
	{ .compatible = "gpio-leds-breath", },
	{},
};

MODULE_DEVICE_TABLE(of, of_gpio_leds_match);


static struct platform_driver gpio_led_driver = {
	.probe		= breath_led_probe,
	.remove		= breath_led_remove,
	.driver		= {
		.name	= "leds-gpio-breath",
		.of_match_table = of_gpio_leds_match,
	},
};


static int  breathled_init(void)
{
	init_timer(&breathled_timer);
	breathled_timer.function = timer_handle;
	add_timer(&breathled_timer);

	platform_driver_register(&gpio_led_driver);
	return 0;
}

static int  breathled_exit(void)
{
	platform_driver_unregister(&gpio_led_driver);
	return 0;
}


module_init(breathled_init);
module_exit(breathled_exit);

MODULE_AUTHOR("Raphael Assenat <raph@8d.com>, Trent Piepho <tpiepho@freescale.com>");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:leds-gpio-breath");

