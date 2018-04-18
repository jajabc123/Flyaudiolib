#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/klog.h>


#define FILE_SIZE 1024*1024*100
#define BUF_SIZE 1024*100

//用于比较文件大小是否超出10M
int filesize_cmp(struct stat *state,int logfd)
{
	int ret = 0;
	ret = lstat("./log.txt",state);//获取文件的状态
	if(ret < 0)
	{
		perror("get state error:");
		return -1;
	}
	
	if((state->st_size != 0)&&(state->st_size <= FILE_SIZE))//文件未满时，文件中间段指针返回
	{
		lseek(logfd,state->st_size,SEEK_SET);
	}
	
	if(state->st_size >= FILE_SIZE)//文件内存大于10M,文件指针返回文件头
	{
		lseek(logfd,0,SEEK_SET);
	}
}

int main(void)
{	
	int logfd = 0,ret = 0,len = 0,devfd = 0;
	char buf[BUF_SIZE] = {0};//文件写内容
	char w_buf[2] = {1,1};//驱动文件写内容
	struct stat state;
	
	devfd = open("/dev/printk_dev", O_RDWR);//打开驱动设备文件
	if(devfd < 0)
	{
		perror("open printk_dev");
		return -1;		
	}
	
	logfd = open("./log.txt",O_CREAT|O_RDWR,0777);//检查日志文件是否存在，如不存在则创建，存在则打开
	if(logfd == -1)
	{
		perror("open error:");
		return -1;
	}
	
	filesize_cmp(&state,logfd);//比较文件大小是否超出10M
	
	while(1)
	{
		ret = write(devfd, w_buf, 2);//写入字符，以调用驱动函数flyaudio_output_write
		if(ret != 2)
		{
			perror("write");
		}
		
		usleep(300*1000);//延时
		memset(buf,0,strlen(buf)+1);//清掉buf中的东西，以免重复打印
		
		ret = klogctl(4,buf,BUF_SIZE);//获取printk函数的信息到buf中，并清除环形缓冲区
		if(ret < 0)
		{
			perror("klogctl error:");
		}
		
		filesize_cmp(&state,logfd);//比较文件大小是否超出10M
		
		ret = write(logfd,buf,strlen(buf)+1);//把内核信息写到文件里面
		printf("ret:%d\r\n",ret);	
		
		sleep(1);

	}
	close(logfd);//关闭文件
	
	return 0;
}