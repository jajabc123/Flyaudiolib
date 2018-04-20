#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/klog.h>
#include <time.h>

#define FILE_SIZE 1024*1024*10
#define BUF_SIZE 1024*100

int main(void)
{	
	int logfd = 0,ret = 0,recfd = 0,offset=0,ret1 = 0;
	char buf[BUF_SIZE] = {0};//文件写内容
	char recbuf[50] = {0};//获取文件指针的buf
	struct stat state;
	
	logfd = open("./log.txt",O_CREAT|O_RDWR,0777);//检查日志文件是否存在，如不存在则创建，存在则打开
	if(logfd == -1)
	{
		perror("open log error:");
		return -1;
	}
	
	recfd = open("./rec.txt",O_CREAT|O_RDWR,0777);//检查指针文件是否存在，如不存在则创建，存在则打开
	if(recfd == -1)
	{
		perror("open rec error:");
		return -1;
	}
	
	read(recfd,recbuf,sizeof(recbuf));//读取存在记录文档的上一次指针的位置
	sscanf(recbuf,"Pointer position:%d\r\n",&offset);
	printf("offset:%d\r\n",offset);
	
	if(offset >= FILE_SIZE)
	{
		lseek(logfd,0,SEEK_SET);//偏移量已超出文件大小，则把文件指针返回到文件头
	}
	else
	{
		lseek(logfd,offset,SEEK_SET);//返回到偏移量的位置
	}

	while(1)
	{
		memset(buf,0,strlen(buf)+1);//清掉buf中的东西，以免重复打印
		
		ret = klogctl(4,buf,BUF_SIZE);//获取printk函数的信息到buf中，并清除环形缓冲区
		if(ret < 0)
		{
			perror("klogctl error:");
		}
		
		if(strlen(buf) != 0)
		{
			ret = write(logfd,buf,strlen(buf)+1);//把内核信息写到文件里面
			printf("ret:%d\r\n",ret);
		}
		
		offset = lseek(logfd,0,SEEK_CUR);//获取文件指针的当前值
		sprintf(recbuf,"Pointer position:%d\r\n",offset);
		
		ret1 = lseek(recfd,0,SEEK_SET);//一直使记录文件的指针在文件头一直覆盖
		write(recfd,recbuf,strlen(recbuf)+1);//写入文件指针当前值*/
	
		if(offset < FILE_SIZE)
		{
			lseek(logfd,offset,SEEK_SET);//返回原位置
		}
		else
		{
			lseek(logfd,0,SEEK_SET);//返回头部
		}
		
		usleep(100000);
		
	}
	
	close(logfd);//关闭文件
	
	return 0;
}