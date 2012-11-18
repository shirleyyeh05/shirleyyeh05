#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "cdata_ioctl.h"

int main(int argc, char *argv[])
{
	int fd;
        pid_t child;

	int num = 0;

	child = fork();   // copy parent's program counter to child 

	/*
	if (argc > 1) num = atoi(argv[1]);
	if (num < 0) num = 0xff;
	*/

	fd = open("/dev/cdata", O_RDONLY);
	if (fd == -1) {
		printf("open /dev/cdata error.");
		return -1;
	}

	/*
	printf("write %d\n", num);
        
	ioctl(fd, IOCTL_EMPTY, num);
	printf("Done, wait 5 seconds ...\n");
	sleep(5);
	*/

	close(fd);
	return 0;
}
