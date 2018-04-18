#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/io.h>

//1.定义一个cdev
static struct cdev flyaudio_output_cdev;

static unsigned int output_major = 100; //主设备号
static unsigned int output_minor = 1; //次设备号
static dev_t  output_num; //设备号

static struct class * output_class;//类
static struct device *output_device;//设备

int flyaudio_output_open(struct inode *inode, struct file *filp)//驱动文件打开
{
	printk("output driver openning\n");
	return 0;
}

ssize_t flyaudio_output_read(struct file *filp, char __user *user_buf, size_t size, loff_t *off)//读驱动文件
{
	//将驱动程序的数据发送给应用程序
	char kbuf[4];
	int ret;
	if(size != 4)
		return -EINVAL;
	ret = copy_to_user(user_buf,kbuf,size);
	if(ret != 0)
		return -EFAULT;
	return size;
}

ssize_t flyaudio_output_write(struct file *filp, const char __user *user_buf, size_t size, loff_t *off)//写驱动文件
{
	//接收用户写下来的数据
	char kbuf[2];
	int ret;
	int i = 0;
	if(size != 2)
		return -EINVAL;
	ret = copy_from_user(kbuf, user_buf, size);//得到用户空间的数据
	if(ret != 0)
		return -EFAULT;
	if((kbuf[0]) == 1)
	{
		if(kbuf[1] == 1)
		for(i=0;i<100;i++)
		{
			printk("flyaudio output write %d success\n",i);
		}
	}
	
	return size;
}

int flyaudio_output_release(struct inode *inode, struct file *filp)//关闭驱动文件
{
	printk("output driver closing\n");
	return 0;
}

//驱动文件函数集分配
static const struct file_operations flyaudio_output_fops = {
	.owner = THIS_MODULE,
	.open = flyaudio_output_open,
	.read = flyaudio_output_read,
	.write = flyaudio_output_write,
	.release = flyaudio_output_release,
};

//入口函数--->安装驱动
static int __init output_init(void)
{
	int ret;
	//3、申请设备号
	if(output_major == 0)
		ret = alloc_chrdev_region(&output_num, output_minor, 1, "output_device");
	else {
		output_num = MKDEV(output_major, output_minor);
		ret = register_chrdev_region(output_num, 1, "output_device");
	}
	if(ret < 0){
		printk("can not get a device number \n");
		return ret;		
	}
	
	//4.字符设备的初始化
	cdev_init(&flyaudio_output_cdev, &flyaudio_output_fops);
	
	//5.将字符设备加入内核
	ret = cdev_add(&flyaudio_output_cdev, output_num, 1);
	if(ret < 0){
		printk("can not add cdev \n");
		unregister_chrdev_region(output_num, 1);
		return ret;		
	}
	
	//6. 创建class
	output_class = class_create(THIS_MODULE, "flyaudio_outputdev");
	if(output_class == NULL){
		printk("class create error\n");
		unregister_chrdev_region(output_num, 1);
		cdev_del(&flyaudio_output_cdev);
		return -EBUSY;
	}
	
	//7.创建device
	output_device = device_create(output_class,NULL,output_num, NULL, "out_dev");
	if(output_device == NULL){
		printk("device create error\n");
		class_destroy(output_class);
		cdev_del(&flyaudio_output_cdev);
		unregister_chrdev_region(output_num, 1);
		
		return -EBUSY;
	}
	printk( "This is Flyaudio init \n");
	
	return 0;
}

//出口函数--->卸载驱动
static void __exit output_exit(void)
{
	
	device_destroy(output_class, output_num);
	class_destroy(output_class);
	cdev_del(&flyaudio_output_cdev);
	unregister_chrdev_region(output_num, 1);
	
	printk("This is Flyaudio exit\n");
}

//驱动程序的入口
module_init(output_init);
//驱动程序的出口
module_exit(output_exit);

//module的描述
MODULE_AUTHOR("flyaudio@163.com");
MODULE_DESCRIPTION("output driver for flyaudio");
MODULE_LICENSE("GPL");
MODULE_VERSION("V1.0");