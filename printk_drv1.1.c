#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include<linux/err.h>            

static struct task_struct *test_task;


int threadfunc(void *data)
{	
	int i = 0;
    while(1)
	{
		for(i = 0;i<10;i++)
		{
			printk("flyaudio output %d\r\n",i);
		}
		
		mdelay(1000);
    }
	return 0;
}


//入口函数--->安装驱动
static int __init output_init(void)
{
	 int err;

    test_task = kthread_create(threadfunc, NULL, "test_task");

    if(IS_ERR(test_task))
	{

    printk("Unable to start kernel thread.\n");

     err = PTR_ERR(test_task);

    test_task =NULL;

    return err;

    }

	wake_up_process(test_task);
	
	
	printk( "This is Flyaudio init \n");
	
	return 0;
}



//出口函数--->卸载驱动
static void __exit output_exit(void)
{
	printk("This is Flyaudio exit\n");
}

//驱动程序的入口
module_init(output_init);
//驱动程序的出口
module_exit(output_exit);

//module的描述
MODULE_AUTHOR("flyaudio@163.com");
MODULE_DESCRIPTION("output driver for flyaudio");
MODULE_LICENSE("DUAL BSD/GPL");
MODULE_VERSION("V1.1");