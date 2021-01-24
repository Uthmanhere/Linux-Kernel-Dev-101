#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>

#define DEBUG__
#define REG_STATIC__
// #define REG_DYNAMIC__

#define DEVICE_NAME  "copy_from_user_or_not"
#define MAJOR_NUMBER  511

static size_t major_number = MAJOR_NUMBER;

static int __init copy_module_init(void);
static void copy_module_cleanup(void);

static struct file_operations fops = {
	.owner = THIS_MODULE
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

