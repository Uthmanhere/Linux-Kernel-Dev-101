#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>

#include <linux/uaccess.h>

#include <linux/slab.h>
#include <linux/highmem.h>

#include "copy_from_user_or_not.h"

#define DEBUG__

static size_t major_number = MAJOR_NUMBER;
static char * p_data;


static int __init copy_module_init(void);
static void copy_module_cleanup(void);

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
static int device_mmap(struct file *, struct vm_area_struct *);
static loff_t device_llseek(struct file *, loff_t, int);


int copy_from_user_or_not(char * p_data, unsigned long u_data, int len)
{
	int ret, i, j, number_of_pages;
	unsigned long offset;
	char * p_user;
	struct page ** pp_pages;

	offset = u_data & (PAGE_SIZE-1);
	number_of_pages = (len+offset) / PAGE_SIZE + 1;
#ifdef DEBUG__
	printk(
		KERN_INFO "COPY_FROM_USER_OR_NOT: Acquiring %d pages with %ld offset.",
		number_of_pages, offset
	);
#endif
	pp_pages = (struct page **)kmalloc(
		number_of_pages * sizeof(struct page *),
		GFP_KERNEL
	);

	if (pp_pages==NULL) {
		printk(
			KERN_ALERT
			"kcopy_from_user: no memory for allocating pages.\n"
		);
		return -ENOMEM;
	}
	//down_read(&current->mm->mmap_sem);
	ret = get_user_pages_fast(
		u_data,
		number_of_pages,
		0,
		pp_pages
	);
	//up_read(&current->mm->mmap_sem);
#ifdef DEBUG__
	printk(
		KERN_INFO "IOCTL_COPY_BUFFER_FROM_USER: %d pages acquired.\n",
		ret
	);
#endif
	ret = 0;
	for (j=0; j<number_of_pages; ++j) {
#ifdef DEBUG__
		printk(
			KERN_INFO
			"IOCTL_COPY_BUFFER_FROM_USER: Mapping page %d/%d\n",
			j+1,
			number_of_pages
		);
#endif
		p_user = (char *)kmap(pp_pages[j]);
#ifdef DEBUG__
		printk(
			KERN_INFO
			"IOCTL_COPY_BUFFER_FROM_USER: Mapped page %d/%d\n",
			j+1,
			number_of_pages
		);
#endif
		if (j==0) {
			p_user += offset;
			for (i=0; (i<PAGE_SIZE-offset) && (i<len); ++i) {
				p_data[i] = p_user[i];
				++ret;
			}
		} else if (j==1) {
			for (i=0; (i<PAGE_SIZE) && (j*PAGE_SIZE+i-offset<len); ++i) {
				p_data[PAGE_SIZE-offset+i] = p_user[i];
				++ret;
			}
		} else {
			for (i=0; i<PAGE_SIZE && j*PAGE_SIZE+i-offset<len; ++i) {
				p_data[j*PAGE_SIZE-offset+i] = p_user[i];
				++ret;
			}
		}
#ifdef DEBUG__
		printk(
			KERN_INFO
			"IOCTL_COPY_BUFFER_FROM_USER: Count of page %d/%d: %d.\n",
			j+1, number_of_pages, ret
		);
#endif
#ifdef DEBUG__
		printk(
			KERN_INFO
			"IOCTL_COPY_BUFFER_FROM_USER: Copied buffer of page %d/%d\n",
			j+1,
			number_of_pages
		);
#endif
		put_page(pp_pages[j]);
#ifdef DEBUG__
		printk(
			KERN_INFO
			"IOCTL_COPY_BUFFER_FROM_USER: Released page %d/%d\n",
			j+1,
			number_of_pages
		);
#endif
		kunmap(pp_pages[j]);
#ifdef DEBUG__
		printk(
			KERN_INFO
			"IOCTL_COPY_BUFFER_FROM_USER: Unmapped page %d/%d\n",
			j+1,
			number_of_pages
		);
#endif
	}
#ifdef DEBUG__
	printk(
		KERN_INFO
		"IOCTL_COPY_BUFFER_FROM_USER: Free page struct array.\n"
	);
#endif
	kfree(pp_pages);
	
	return BUFF_LEN-ret;
	
}

long device_ioctl(struct file * filp, unsigned int ioctl_type, unsigned long ioctl_param)
{
	int ret;

	switch(ioctl_type) {
	case IOCTL_COPY_BUFFER_FROM_USER:
		ret = copy_from_user_or_not(p_data, ioctl_param, BUFF_LEN);
#ifdef DEBUG__
		printk(
			KERN_INFO
			"COPY_FROM_USER_OR_NOT: IOCTL_COPY_BUFFER_FROM_USER: Copied %d bytes.\n",
			BUFF_LEN-ret
		);
		/*
		print_hex_dump(
			KERN_INFO, "", DUMP_PREFIX_ADDRESS,
			32, 1, p_data, BUFF_LEN, 1
		);
		*/
#endif
		return ret;
	case IOCTL_COPY_BUFFER_TO_USER:
		ret = copy_to_user((char *)ioctl_param, p_data, BUFF_LEN);
#ifdef DEBUG__
		printk(
			KERN_INFO
			"COPY_FROM_USER_OR_NOT: IOCTL_COPY_BUFFER_TO_USER: Copied %d bytes.\n",
			BUFF_LEN-ret
		);
#endif
		return ret;
	default:
		return -ENOSYS;
	}

	return 0;
}

loff_t device_llseek(struct file * filp, loff_t offset, int whence)
{

	switch (whence) {
	case 0: /* SEEK_SET */
		break;
	case 1: /* SEEK_CUR */
		break;
	case 2: /* SEEK_END */
		break;
	default:
		return -EINVAL;

	}
	return offset+((loff_t)p_data & (PAGE_SIZE-1));
}

int device_mmap(struct file * filp, struct vm_area_struct * vma)
{
	int ret;
	unsigned long offset;
#ifdef DEBUG__
	printk(
		KERN_INFO
		"COPY_FROM_USER_OR_NOT: mmap initiated.\n"
	);
#endif

	offset = (unsigned long)p_data & (PAGE_SIZE-1);
    // vma->vm_flags |= VM_IO | VM_PFNMAP | VM_DONTEXPAND | VM_DONTDUMP;
	vma->vm_flags |= VM_LOCKED;
	ret = remap_pfn_range(
		vma, vma->vm_start, virt_to_phys(p_data) >> PAGE_SHIFT,
		BUFF_LEN+offset, /*vma->vm_page_prot |*/ PAGE_SHARED
	);

	print_hex_dump(
		KERN_INFO, "", DUMP_PREFIX_ADDRESS,
		32, 1, p_data, BUFF_LEN, 1
	);

	if (ret < 0) {
		printk(
			KERN_WARNING
			"COPY_FROM_USER_OR_NOT: Device memory mapping failed.\n"
		);
	}
#ifdef DEBUG__
	printk(
		KERN_INFO
		"COPY_FROM_USER_OR_NOT: mmap succeeded.\n"
	);
#endif
	return ret;
}


static int device_open(struct inode * inode, struct file * filp)
{
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
		"COPY_FROM_USER_OR_NOT: Reads on device file are not supported.\n"
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
	.owner          = THIS_MODULE,
	.open           = device_open,
	.release        = device_release,
	.read           = device_read,
	.write          = device_write,
	.unlocked_ioctl = device_ioctl,
	.mmap           = device_mmap,
	.llseek         = device_llseek
};

static int __init copy_module_init(void)
{

	int ret;

	p_data = (char *) kmalloc(BUFF_LEN*sizeof(char), GFP_KERNEL);
	if (p_data == NULL) {
		printk(KERN_WARNING "COPY_FROM_USER_OR_NOR: Unable to allocate buffer.\n");
		return -ENOMEM;
	}

#ifdef DEBUG__
	printk(KERN_INFO "COPY_FROM_USER_OR_NOT: Initialized.\n");
#endif

	ret = register_chrdev(MAJOR_NUMBER, DEVICE_NAME, &fops);


	return 0;
}

static void copy_module_cleanup(void)
{
	if (p_data)
		kfree(p_data);
	unregister_chrdev(major_number, DEVICE_NAME);
#ifdef DEBUG__
	printk(KERN_INFO "COPY_FROM_USER_OR_NOT: Terminated.\n");
#endif
}

module_init(copy_module_init);
module_exit(copy_module_cleanup);
MODULE_LICENSE("GPL");
