#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
/* Matching struct defintion the same way we defined on
   _IOW and _IOR
 */
struct ioctl_arg {
	unsigned int v;
};

#define IOC_MAGIC '\x66'
#define IOCTL_VALSET _IOW(IOC_MAGIC, 0, struct ioctl_arg)
#define IOCTL_VALGET _IOR(IOC_MAGIC, 0, struct ioctl_arg)

int main(void) {
	/*
	  Thanks to Unix design /dev exposes kernel drivers through
	  a regular file, we can use fds and route calls via the fops struct
	*/
	int fd = open("/dev/ioctl_module", O_RDWR);
	if (fd < 0) {
		perror("open");
		return 1;
	}
	struct ioctl_arg data;
	data.v = 0x12;
	// write to driver value
	if (ioctl(fd, IOCTL_VALSET, &data) < 0)
		perror("IOCTL_VALSET");
	else
		printf("value set 0x%x\n", data.v);
	// reset value to validate call
	data.v = 0;
	if (ioctl(fd, IOCTL_VALGET, &data) < 0)
		perror("IOCTL_VALGET");
	else
		printf("value read 0x%x \n", data.v);

	close(fd);
	return 0;
}
