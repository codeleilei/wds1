#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <poll.h>
#include <wait.h>
#include <signal.h>

int fd;
void my_handle(int signum)
{
	int ret;
	char val[3];
	
	printf("this is signal handle!!!\n");
	
	ret=read(fd,val,3);
	if(ret < 0)
		perror("read");
	printf("LED1:%d  LED2:%d  LED3:%d\n",val[0],val[1],val[2]);

}
int main(int argc,char **argv)
{
	int Oflag;

	signal(SIGIO,my_handle);
	
	fd=open("/dev/five_key",O_RDWR);
	if(fd < 0)
	{
		perror("open");
		return -1;
	}

	fcntl(fd,F_SETOWN,getpid());
	Oflag = fcntl(fd,F_GETFL);
	fcntl(fd,F_SETFL,Oflag | FASYNC);

	while(1)
	{
		sleep(5000);
	}
	return 0;
}

