/*
 * Copyright (C) 2008 Anant Narayanan <anant@kix.in>
 * Plan 9 system call implementations
 */

#include <linux/fs.h>
#include <linux/time.h>
#include <linux/file.h>
#include <linux/mount.h>
#include <linux/dcache.h>
#include <linux/syscalls.h>

#include <asm/current.h>
#include <asm/uaccess.h>
#include <asm/processor.h>

asmlinkage long sys_plan9_unimplemented(struct pt_regs regs)
{
	if (printk_ratelimit())
		printk(KERN_ALERT "P9: %ld called but unimplemented!\n",
			regs.eax);
	return 0;
}

asmlinkage long sys_plan9_deprecated(struct pt_regs regs)
{
	if (printk_ratelimit())
		printk(KERN_INFO "P9: syscall number %ld DEPRECATED!\n",
			regs.eax);
	return 0;
}

asmlinkage long sys_plan9_exits(struct pt_regs regs)
{
	printk(KERN_INFO "P9: Syscall %ld exits called!\n", regs.eax);
	return sys_exit(1);
}

asmlinkage long sys_plan9_chdir(struct pt_regs regs)
{
	unsigned long arg1;
	unsigned long *addr = (unsigned long *)regs.esp;
	printk(KERN_INFO "P9: Syscall %ld chdir called!\n", regs.eax);

	get_user(arg1, ++addr);
	
	return sys_chdir((char __user *)arg1);
}

asmlinkage long sys_plan9_close(struct pt_regs regs)
{
	unsigned long arg1;
	unsigned long *addr = (unsigned long *)regs.esp;
	printk(KERN_INFO "P9: Syscall %ld close called!\n", regs.eax);

	get_user(arg1, ++addr);
	
	return sys_close(arg1);
}

asmlinkage long sys_plan9_dup(struct pt_regs regs)
{
	unsigned long arg1, arg2;
	unsigned long *addr = (unsigned long *)regs.esp;
	printk(KERN_INFO "P9: Syscall %ld dup called!\n", regs.eax);

	get_user(arg1, ++addr);
	get_user(arg2, ++addr);
	return sys_dup2(arg1, arg2);
}

asmlinkage long sys_plan9_open(struct pt_regs regs)
{
	unsigned long arg1, arg2;
	unsigned long *addr = (unsigned long *)regs.esp;
	printk(KERN_INFO "P9: Syscall %ld open called!\n", regs.eax);

	get_user(arg1, ++addr);
	get_user(arg2, ++addr);

	/* FIXME: Mode needs to be check in all combos! */
	return sys_open((const char __user *)arg1, arg2, (int) NULL);
}

asmlinkage long sys_plan9_sleep(struct pt_regs regs)
{
	int rval;
	unsigned long arg1;
	struct timespec time;
	
	unsigned long *addr = (unsigned long *)regs.esp;
	printk(KERN_INFO "P9: Syscall %ld sleep called!\n", regs.eax);
	
	get_user(arg1, ++addr);
	
	/* Milliseconds to seconds */
	time.tv_sec = (time_t)arg1 / 1000;
	arg1 -= time.tv_sec * 1000;
	
	/* Milliseconds to nanoseconds */
	time.tv_nsec = arg1 * 1000000;
	
	printk(KERN_INFO "P9: sleep: %ldms resolved to %lds and %ldns\n",
				arg1, time.tv_sec, time.tv_nsec);
	rval = sys_nanosleep(&time, &time);
	
	if (rval == 0)
		return 0;
	else
		return -1;
}

asmlinkage long sys_plan9_create(struct pt_regs regs)
{
	unsigned long arg1, arg2, arg3;
	unsigned long *addr = (unsigned long *)regs.esp;
	printk(KERN_INFO "P9: Syscall %ld create called!\n", regs.eax);
	
	get_user(arg1, ++addr);
	get_user(arg2, ++addr);
	get_user(arg3, ++addr);
	
	/* TODO: check mode */
	return sys_open((const char __user *)arg1, arg2 | O_CREAT, arg3);
}

/* Original code is (C) Alexander Viro, linux-kernel, 12th Aug 2000
 * Original code was modified to fit this structure correctly.
 */
asmlinkage long sys_plan9_fd2path(struct pt_regs regs)
{
	char *cwd;
	int fd, error;
	char __user *buf;

	struct file *file;
	struct vfsmount *mnt;
	struct dentry *dentry;
	
	unsigned long ba, size, len;
	unsigned long *addr = (unsigned long *)regs.esp;
	
	printk(KERN_INFO "P9: Syscall %ld fd2path called!\n", regs.eax);
	
	char *page = (char *) __get_free_page(GFP_USER);
	if (!page)
		return -ENOMEM;
	
	get_user(fd, ++addr);
	get_user(ba, ++addr);
	get_user(size, ++addr);

	buf = (char __user *)ba;
	
	file = fget(fd);
	if (!file)
		return -EBADF;

	mnt = mntget(file->f_vfsmnt);
	dentry = dget(file->f_dentry);
	fput(file);
	
	cwd = d_path(dentry, mnt, page, PAGE_SIZE);
	error = -ERANGE;
	len = PAGE_SIZE + page - cwd;
	if (len <= size) {
		error = len;
		if (copy_to_user(buf, cwd, len))
			error = -EFAULT;
	}

	dput(dentry);
	mntput(mnt);

	free_page((unsigned long) page);

	return error;
}

asmlinkage long sys_plan9_brk(struct pt_regs regs)
{
	unsigned long arg1;
	unsigned long *addr = (unsigned long *) regs.esp;
	printk(KERN_INFO "P9: Syscall %ld brk called!\n", regs.eax);

	get_user(arg1, ++addr);
	
	return sys_brk(arg1);
}

asmlinkage long sys_plan9_remove(struct pt_regs regs)
{
	unsigned long arg1;
	unsigned long *addr = (unsigned long *) regs.esp;
	printk(KERN_INFO "P9: Syscall %ld remove called!\n", regs.eax);

	get_user(arg1, ++addr);

	return sys_unlink((const char __user *)arg1);
}

asmlinkage long sys_plan9_seek(struct pt_regs regs)
{
	loff_t offset;
	unsigned long arg1, arg2, arg3;
	unsigned long *addr = (unsigned long *)regs.esp;
	printk(KERN_INFO "P9: Syscall %ld seek called!\n", regs.eax);

	get_user(arg1, ++addr);
	get_user(arg2, ++addr);
	addr = addr + 2;
	copy_from_user(&offset, addr, sizeof(loff_t));
	get_user(arg3, ++addr);

	return sys_lseek(arg2, (off_t) offset, arg3);
}

asmlinkage long sys_plan9_pread(struct pt_regs regs)
{
	loff_t offset;
	unsigned long arg1, arg2, arg3;
	unsigned long *addr = (unsigned long *)regs.esp;
	printk(KERN_INFO "P9: Syscall %ld pread called!\n", regs.eax);

	get_user(arg1, ++addr);
	get_user(arg2, ++addr);
	get_user(arg3, ++addr);
	addr = addr + 2;
	copy_from_user(&offset, addr, sizeof(loff_t));

	printk(KERN_INFO "P9: pread: offset: %llx\n", offset);
	if (offset == 0xffffffff) {
		printk(KERN_INFO "P9: pread: calling with %lx, %lx, %lx\n", arg1, arg2, arg3);
		return sys_read(arg1, (char __user *)arg2, arg3);
	} else {
		return sys_pread64(arg1, (char __user *)arg2, arg3, offset); 
	}
}

asmlinkage long sys_plan9_pwrite(struct pt_regs regs)
{
	loff_t offset;
	unsigned long arg1, arg2, arg3;
	unsigned long *addr = (unsigned long *)regs.esp;
	printk(KERN_INFO "P9: Syscall %ld pwrite called!\n", regs.eax);

	get_user(arg1, ++addr);
	get_user(arg2, ++addr);
	get_user(arg3, ++addr);
	addr = addr + 2;
	copy_from_user(&offset, addr, sizeof(loff_t));

	printk(KERN_INFO "P9: pwrite: offset: %llx\n", offset);
	if (offset == 0xffffffff) {
		printk(KERN_INFO "P9: pwrite: calling with %lx, %lx, %lx\n", arg1, arg2, arg3);
		return sys_write(arg1, (char __user *)arg2, arg3);
	} else {
		return sys_pwrite64(arg1, (char __user *)arg2, arg3, offset);
	}
}

