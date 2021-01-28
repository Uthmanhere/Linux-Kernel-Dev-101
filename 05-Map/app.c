#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include "copy_from_user_or_not.h"
#include "hexdump.h"

int main() {
	int ret;
	int fd = open(DEVICE_NAME, 0);
	if (fd < 0) {
		puts("Device file '/dev/copy_from_user_or_not' does not exist.");
		return -1;
	}

	char p_data[BUFF_LEN]     = { 'A' };
	char p_data_ret[BUFF_LEN] = { '\0' };

	char c = 'A';
	for (int i=0; i<BUFF_LEN; ++i) {
		if (c>'Z')
			c = 'A';
		p_data[i] = c++;
	}

	ret = ioctl(fd, IOCTL_COPY_BUFFER_FROM_USER, p_data);
	if (ret!=0) {
		puts("Could not copy data to kernel.");
		return -1;
	}
	ret = ioctl(fd, IOCTL_COPY_BUFFER_TO_USER, p_data_ret);
	if (ret!=0) {
		puts("Could not copy data from kernel.");
		return -1;
	}
	for (int i=0; i<BUFF_LEN; ++i) {
		if (p_data[i]!=p_data_ret[i]) {
			printf("Did not return correct data on index: %d\n", i);
		}
	}
	
	
	char * p_data_map = mmap(NULL, BUFF_LEN, PROT_READ, MAP_FILE | MAP_SHARED, fd, 0);
	p_data_map = (char *)lseek(fd, (loff_t)p_data_map, 0);

	hexdump(p_data_map, BUFF_LEN);
	munmap(p_data_map, BUFF_LEN);
	close(fd);

	return 0;
}
