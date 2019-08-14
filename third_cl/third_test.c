#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>



int main(int argc,char **argv)
{
	int fd,f;
	char LED_STAT[3];
	
	fd=open("/dev/third_key",O_RDWR);
	printf("this is test!!!\n");

	if(fd < 0)
	{
		perror("open");
		return -1;
	}
	while(1)
	{
		f=read(fd,LED_STAT,3);
		if(fd < 0)
		{
			perror("read");
			return -1;
		}
		printf("LED1:%d,led2:%d,led3:%d",LED_STAT[0],LED_STAT[1],LED_STAT[2]);
	}
	return 0;
}

