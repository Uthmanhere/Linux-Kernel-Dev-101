#define DEVICE_NAME "/dev/copy_from_user_or_not"
#define MAJOR_NUMBER 511

#define BUFF_LEN     256

#define IOCTL_COPY_BUFFER_FROM_USER _IOR  (MAJOR_NUMBER, 0, char  *)
#define IOCTL_COPY_BUFFER_TO_USER   _IOWR (MAJOR_NUMBER, 1, char  *)



