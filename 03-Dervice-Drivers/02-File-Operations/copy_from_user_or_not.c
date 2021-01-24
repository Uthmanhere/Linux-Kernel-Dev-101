#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>

#define DEBUG__
//#define REG_STATIC__
#define REG_DYNAMIC__

#define DEVICE_NAME  "copy_from_user_or_not"
#define MAJOR_NUMBER  511

static size_t major_number = MAJOR_NUMBER;
static int open_flag = 0;
static int open_count = 0;


static int __init copy_module_init(void);
static void copy_module_cleanup(void);



static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);


static int device_open(struct inode * inode, struct file * filp)
{
	try_module_get(THIS_MODULE);

	if (!open_flag) {
		++open_flag;
		++open_count;
	} else {
		printk(
			KERN_WARNING
			"COPY_FROM_USER_OR_NOT: Could not open device file for module.\n"
		);
	}

#ifdef DEBUG__
	printk(
		KERN_INFO
		"COPY_FROM_USER_OR_NOT: Device file for module opened.\n"
	);
#endif
	return 0;
}

static int device_release(struct inode * inode, struct file * filp)
{
	module_put(THIS_MODULE);
	--open_flag;

#ifdef DEBUG__
	printk(
		KERN_INFO
		"COPY_FROM_USER_OR_NOT: Device file for module released.\n"
	);
#endif
	return 0;
}

static ssize_t device_read(struct file * filp, char * buff, size_t length, loff_t * offset)
{
	printk(
		KERN_INFO
		"COPY_FROM_USER_OR_NOT: Device has been opened %d times\n",
		open_count
	);

	return 0;
}

static ssize_t device_write(struct file * filp, const char * buff, size_t len, loff_t * off)
{
	printk(
		KERN_INFO
		"COPY_FROM_USER_OR_NOT: Writes on device file are not supported.\n"
	);
	return 0;
}

static struct file_operations fops = {
	.owner   = THIS_MODULE,
	.open    = device_open,
	.release = device_release,
	.read    = device_read,
	.write   = device_write
};

static int __init copy_module_init(void)
{

	int ret;

#ifdef DEBUG__
	printk(KERN_INFO "COPY_FROM_USER_OR_NOT: Initialized.\n");
#endif

#ifdef REG_STATIC__
	ret = register_chrdev(MAJOR_NUMBER, DEVICE_NAME, &fops);
#endif
#ifdef REG_DYNAMIC__
	major_number = register_chrdev(0, DEVICE_NAME, &fops);
	if (major_number < 0) {
		printk(
			KERN_ALERT
			"COPY_FROM_USER_OR_NOT: Could not register module.\n"
		);
		return ret;
	}
	printk(
		KERN_INFO
		"COPY_FROM_USER_OR_NOT: Watashi no bango wa %ld desu.\n",
		major_number
	);
#endif

	return 0;
}

static void copy_module_cleanup(void)
{
	unregister_chrdev(major_number, DEVICE_NAME);
#ifdef DEBUG__
	printk(KERN_INFO "COPY_FROM_USER_OR_NOT: Terminated.\n");
#endif
}

module_init(copy_module_init);
module_exit(copy_module_cleanup);
MODULE_LICENSE("GPL");

