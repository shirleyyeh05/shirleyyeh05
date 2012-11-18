#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "cdata_ioctl.h"

int main(int argc, char *argv[])
{
	int fd;
        pid_t child;

	//int num = 0;

	child = fork();   // copy parent's program counter to child 

	/*
	if (argc > 1) num = atoi(argv[1]);
	if (num < 0) num = 0xff;
	*/

	fd = open("/dev/cdata", O_RDWR);
	ioctl(fd, IOCTL_EMPTY, 0);

	if (child != 0)
		write(fd, "ABCDE", 5);
	else
		write(fd, "12345", 5);
	
	ioctl(fd, IOCTL_SYNC, 0);

	//printf("Done, wait 5 seconds ...\n");
	//sleep(5);
	
	close(fd);
	return 0;
}
