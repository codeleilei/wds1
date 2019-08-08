#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

struct led{
	char num;
	char state;
};

int main(int argc,char **argv)
{
	int fd,f,i;
	char key[3];
	struct led led_stat[3]; 
	
	fd=open("/dev/cl_sec",O_RDWR);

	if(fd < 0)
	{
		perror("open");
		return -1;
	}

	while(1)
	{
		f = read(fd,key,3);
		printf("key[0]=%c,key[1]=%c,key[2]=%c\n",key[0]+'0',key[1]+'0',key[2]+'0');
		if(fd < 0)
		{
			perror("read");
			return -1;
		}
		
		for(i=0; i < 4 ; i++)
		{	
		led_stat[i].num=i;
			if( key[i] == 0 ) 
			{
				led_stat[i].state = 1;
				write(fd,&led_stat[i],sizeof(struct led));
			}
			else
			{
				led_stat[i].state = 0;
				write(fd,&led_stat[i],sizeof(struct led));	
			}			
		}
	}
	return 0;
}
