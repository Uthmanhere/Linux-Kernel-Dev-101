#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "copy_from_user_or_not.h"

int main() {
	int fd = open(DEVICE_NAME, 0);
	if (fd < 0) {
		puts("Device file '/dev/copy_from_user_or_not' does not exist.");
		return -1;
	}

	printf(
		"The device file has been opened %d times.\n",
		(int)ioctl(fd, IOCTL_GET_OPEN_COUNT, 0)
	);

	int ret = (int)ioctl(fd, IOCTL_SET_OPEN_COUNT, 0);
	if (ret!=0) {
		puts("Failed to (re)set device open count.");
		return -1;
	}

	puts("The device count for file has been reset.");


	return 0;
}
