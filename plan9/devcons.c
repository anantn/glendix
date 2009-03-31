/**
 * Plan 9 '#c' emulation.
 * Let's start with /dev/pid
 */
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/syscalls.h>
#include <linux/miscdevice.h>

MODULE_AUTHOR("Anant Narayanan <anant@kix.in>");
MODULE_LICENSE("GPL");

static ssize_t pid_read(struct file *, char __user *, size_t, loff_t *);

static const struct file_operations pid_fops = {
	.owner = THIS_MODULE,
	.read = pid_read
	/* .write to /dev/pid doesn't make sense? */
};

static struct miscdevice pid_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "pid",
	.fops = &pid_fops
};

static ssize_t pid_read(struct file *f, char __user *buf,
							size_t count, loff_t *offset)
{
	int ret = 0;
	char pidbuf[4];
	unsigned long readcount;
	
	if (*offset != 0) {
		ret = 0;
	} else {
		ret = scnprintf(pidbuf, 4, "%ld", sys_getpid());
		readcount = min(count, (size_t)ret);
		
		if (!copy_to_user(buf, pidbuf, readcount)) {
			*offset += readcount;
			ret = readcount;
		} else {
			ret = -EFAULT;
		}
	}
	
	return ret;
}

static int __init cons_init(void)
{
	return misc_register(&pid_dev);
}

static void __exit cons_exit(void)
{
	misc_deregister(&pid_dev);
}

module_init(cons_init);
module_exit(cons_exit);
