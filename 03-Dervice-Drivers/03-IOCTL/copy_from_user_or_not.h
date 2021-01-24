#define DEVICE_NAME "/dev/copy_from_user_or_not"
#define MAJOR_NUMBER 511

#define IOCTL_SET_OPEN_COUNT _IOR  (MAJOR_NUMBER, 0, int)
#define IOCTL_GET_OPEN_COUNT _IOR  (MAJOR_NUMBER, 1, int)



