#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <poll.h>

int main(int argc,char **argv)
{
	int fd,ret;
	char val[3];
	struct pollfd fds[1];
	printf("open!!!\n");
	fd=open("/dev/forth_key",O_RDWR);
	if(fd < 0)
	{
		perror("open");
		return -1;
	}
	printf("open!!! \n");
	fds[0].fd=fd;
	fds[0].events=POLLIN;
	while(1)
	{
		ret=poll(fds,1,5000);
		if(ret == 0) 
			printf("time out!!! \n");
		else
		{
			ret=read(fd,val,3);
			if(ret < 0)
			{
				perror("read");
				return -1;
			}
			printf("LED1:%d  LED2:%d  LED3:%d\n",val[0],val[1],val[2]);
		}
		
	}
	return 0;
}

