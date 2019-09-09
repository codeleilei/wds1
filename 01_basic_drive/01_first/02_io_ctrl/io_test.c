#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
int main(int argc, char **argv)
{
	int fd;
	int f,val;
	fd = open("dev/io_ctl",O_RDWR);///
	if(fd < 0 ){
		printf("open fail\n");
		return -1;
	}
	if(argc < 2){
		printf("please input 1 parm(1~5)\n");
		return -1;
	}
	if (strcmp(argv[1], "1") == 0)
			val= 1;
	if (strcmp(argv[1], "2") == 0)
			val= 2;
	if (strcmp(argv[1], "3") == 0)
			val= 3;
	if (strcmp(argv[1], "4") == 0)
			val= 4;  //all up
	if (strcmp(argv[1], "5") == 0)
			val= 5; // all down

	f=write(fd,&val,4);

	if(f < 0 ){
		printf("write fail\n");
		return -1;
	}
	close(fd);
	return 0;
}
