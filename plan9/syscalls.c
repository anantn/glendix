/*
 * Copyright (C) 2008 Anant Narayanan <anant@kix.in>
 * Plan 9 system call implementations
 */

#include <linux/fs.h>
#include <linux/time.h>
#include <linux/file.h>
#include <linux/mount.h>
#include <linux/dcache.h>
#include <linux/string.h>
#include <linux/fsnotify.h>
#include <linux/syscalls.h>

#include <asm/current.h>
#include <asm/uaccess.h>
#include <asm/syscalls.h>
#include <asm/processor.h>

#include "p9_constants.h"

asmlinkage long sys_plan9_unimplemented(struct pt_regs regs)
{
	if (printk_ratelimit())
		printk(KERN_ALERT "P9: %ld called but unimplemented!\n",
			regs.ax);
	return 0;
}

asmlinkage long sys_plan9_deprecated(struct pt_regs regs)
{
	if (printk_ratelimit())
		printk(KERN_INFO "P9: syscall number %ld DEPRECATED!\n",
			regs.ax);
	return 0;
}

asmlinkage long sys_plan9_exits(struct pt_regs regs)
{
	printk(KERN_INFO "P9: Syscall %ld exits called!\n", regs.ax);
	return sys_exit(1);
}

asmlinkage long sys_plan9_chdir(struct pt_regs regs)
{
	unsigned long dirname;
	unsigned long *addr = (unsigned long *)regs.sp;
	printk(KERN_INFO "P9: Syscall %ld chdir called!\n", regs.ax);

	get_user(dirname, ++addr);
	
	return sys_chdir((char __user *)dirname);
}

asmlinkage long sys_plan9_close(struct pt_regs regs)
{
	unsigned long fd;
	unsigned long *addr = (unsigned long *)regs.sp;
	printk(KERN_INFO "P9: Syscall %ld close called!\n", regs.ax);

	get_user(fd, ++addr);
	
	return sys_close(fd);
}

asmlinkage long sys_plan9_dup(struct pt_regs regs)
{
	unsigned long oldfd, newfd;
	unsigned long *addr = (unsigned long *)regs.sp;
	printk(KERN_INFO "P9: Syscall %ld dup called!\n", regs.ax);

	get_user(oldfd, ++addr);
	get_user(newfd, ++addr);
	
	if (newfd == -1) {
		/* User requested lowest available descriptor */
		return sys_dup(oldfd);
	} else {
		/* User requested newfd to be the new descriptor
		 * FIXME: Plan 9 ensures that newfd is no more than 20 larger
		 * than the largest fd currently in use by the program.
		 */
		return sys_dup2(oldfd, newfd);
	}
}

asmlinkage long sys_plan9_open(struct pt_regs regs)
{
	int fd, len;
	struct file *f;
	char path[PATH_MAX + 1];
	unsigned long file, omode;
	unsigned long *addr = (unsigned long *)regs.sp;
	printk(KERN_INFO "P9: Syscall %ld open called!\n", regs.ax);

	get_user(file, ++addr);
	get_user(omode, ++addr);

	/* Special case for '#c/pid' */
	if ((len = strncpy_from_user(path, file, PATH_MAX)) < 0) {
		return -EFAULT;
	}
	path[len] = '\0';

	if (strncmp(path, "#c/pid", 6) == 0) {
		strncpy(path, "/dev/pid\0", 9);
		printk(KERN_INFO "P9: open for #c/pid received, changed to %s!\n", path);
	} else {
		printk(KERN_INFO "P9: open for %s received\n", path);
	}
	
	fd = PTR_ERR(path);

	if (!IS_ERR(path)) {
		fd = get_unused_fd();
		if (fd >= 0) {
			f = do_filp_open(AT_FDCWD, path, (int)NULL, omode);
			if (IS_ERR(f)) {
				put_unused_fd(fd);
				fd = PTR_ERR(f);
			} else {
				fsnotify_open(f->f_path.dentry);
				fd_install(fd, f);
			}
		}
		putname(path);
	}

	return (long)fd;
}

asmlinkage long sys_plan9_sleep(struct pt_regs regs)
{
	int rval;
	struct timespec time;
	unsigned long millisecs;
	unsigned long *addr = (unsigned long *)regs.sp;
	printk(KERN_INFO "P9: Syscall %ld sleep called!\n", regs.ax);
	
	get_user(millisecs, ++addr);
	
	/* Milliseconds to seconds */
	time.tv_sec = (time_t)millisecs / 1000;
	millisecs -= time.tv_sec * 1000;
	/* Milliseconds to nanoseconds */
	time.tv_nsec = millisecs * 1000000;
	
	printk(KERN_INFO "P9: sleep: %ldms resolved to %lds and %ldns\n",
				millisecs, time.tv_sec, time.tv_nsec);
	rval = sys_nanosleep(&time, &time);
	
	if (rval == 0)
		return 0;
	else
		return -1;
}

asmlinkage long sys_plan9_create(struct pt_regs regs)
{
	unsigned long file, omode, perm;
	unsigned long *addr = (unsigned long *)regs.sp;
	printk(KERN_INFO "P9: Syscall %ld create called!\n", regs.ax);
	
	get_user(file, ++addr);
	get_user(omode, ++addr);
	get_user(perm, ++addr);
	
	/* TODO: check modes */
	return sys_open((const char __user *)file, omode | O_CREAT, perm);
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
	
	unsigned long abuf, nbuf, len;
	char *page = (char *) __get_free_page(GFP_USER);
	unsigned long *addr = (unsigned long *)regs.sp;
	printk(KERN_INFO "P9: Syscall %ld fd2path called!\n", regs.ax);
	
	if (!page)
		return -ENOMEM;
	
	get_user(fd, ++addr);
	get_user(abuf, ++addr);
	get_user(nbuf, ++addr);

	buf = (char __user *)abuf;
	
	file = fget(fd);
	if (!file)
		return -EBADF;

	mnt = mntget(file->f_vfsmnt);
	dentry = dget(file->f_dentry);
	fput(file);
	
	cwd = __d_path(dentry, mnt, page, PAGE_SIZE);
	error = -ERANGE;
	len = PAGE_SIZE + page - cwd;
	if (len <= nbuf) {
		error = len;
		if (copy_to_user(buf, cwd, len))
			error = -EFAULT;
	}

	dput(dentry);
	mntput(mnt);

	free_page((unsigned long) page);

	return error;
}

/* FIXME: Find out if this is brk_ or sbrk! */
asmlinkage long sys_plan9_brk(struct pt_regs regs)
{
	unsigned long incr;
	unsigned long *addr = (unsigned long *) regs.sp;
	printk(KERN_INFO "P9: Syscall %ld brk called!\n", regs.ax);

	get_user(incr, ++addr);
	
	return sys_brk(incr);
}

asmlinkage long sys_plan9_remove(struct pt_regs regs)
{
	unsigned long file;
	unsigned long *addr = (unsigned long *) regs.sp;
	printk(KERN_INFO "P9: Syscall %ld remove called!\n", regs.ax);

	get_user(file, ++addr);

	return sys_unlink((const char __user *)file);
}

/* FIXME: Plan9's seek uses vlong, we ignore n here! */
asmlinkage long sys_plan9_seek(struct pt_regs regs)
{
	loff_t offset;
	unsigned long fd, n, type;
	unsigned long *addr = (unsigned long *)regs.sp;
	printk(KERN_INFO "P9: Syscall %ld seek called!\n", regs.ax);

	get_user(n, ++addr);
	get_user(fd, ++addr);
	addr = addr + 2;
	copy_from_user(&offset, addr, sizeof(loff_t));
	get_user(type, ++addr);

	return sys_lseek(fd, (off_t) offset, type);
}

asmlinkage long sys_plan9_pread(struct pt_regs regs)
{
	loff_t offset;
	unsigned long fd, buf, nbytes;
	unsigned long *addr = (unsigned long *)regs.sp;
	printk(KERN_INFO "P9: Syscall %ld pread called!\n", regs.ax);

	get_user(fd, ++addr);
	get_user(buf, ++addr);
	get_user(nbytes, ++addr);
	addr = addr + 2;
	copy_from_user(&offset, addr, sizeof(loff_t));

	printk(KERN_INFO "P9: pread: offset: %llx\n", offset);
	if (offset == 0xffffffff) {
		printk(KERN_INFO "P9: pread: calling with %lx, %lx, %lx\n",
							fd, buf, nbytes);
		return sys_read(fd, (char __user *)buf, nbytes);
	} else {
		return sys_pread64(fd, (char __user *)buf, nbytes, offset); 
	}
}

asmlinkage long sys_plan9_pwrite(struct pt_regs regs)
{
	loff_t offset;
	unsigned long fd, buf, nbytes;
	unsigned long *addr = (unsigned long *)regs.sp;
	printk(KERN_INFO "P9: Syscall %ld pwrite called!\n", regs.ax);

	get_user(fd, ++addr);
	get_user(buf, ++addr);
	get_user(nbytes, ++addr);
	addr = addr + 2;
	copy_from_user(&offset, addr, sizeof(loff_t));

	printk(KERN_INFO "P9: pwrite: offset: %llx\n", offset);
	if (offset == 0xffffffff) {
		printk(KERN_INFO "P9: pwrite: calling with %lx, %lx, %lx\n",
							fd, buf, nbytes);
		return sys_write(fd, (char __user *)buf, nbytes);
	} else {
		return sys_pwrite64(fd, (char __user *)buf, nbytes, offset);
	}
}

asmlinkage long sys_plan9_rfork(struct pt_regs regs)
{
	long ret = -1;
	int clone_flags = 1;
	unsigned long flags;
	unsigned long *addr = (unsigned long *)regs.sp;

	printk(KERN_INFO "P9: Syscall %ld rfork called!\n", regs.ax);
	get_user(flags, ++addr);
	printk(KERN_INFO "P9: rfork called with %lx\n", flags);

	/* Check for invalid flag combinations */
	if ((flags & (RFFDG | RFCFDG)) == (RFFDG | RFCFDG))
		return -EINVAL;
	if ((flags & (RFNAMEG | RFCNAMEG)) == (RFNAMEG | RFCNAMEG))
		return -EINVAL;
	if ((flags & (RFENVG | RFCENVG)) == (RFENVG | RFCENVG))
		return -EINVAL;

	if (flags & RFPROC) {
		if (flags & (RFMEM | RFNOWAIT))
			return -EINVAL;
		
		if (flags & RFNOWAIT) {
			printk(KERN_INFO "rfork with RFNOWAIT unimplemented!\n");	
		}

		if (flags & RFNAMEG) {
			clone_flags |= (CLONE_NEWNS | CLONE_FILES);
		} else if (flags & RFCNAMEG) {
			clone_flags |= (CLONE_FILES);
		}

		if (flags & RFNOMNT) {
			printk(KERN_INFO "rfork with RFNOMNT unimplemented!\n");
		}
		if (flags & RFENVG) {
			printk(KERN_INFO "rfork with RFENVG unimplemented!\n");
		} else if (flags & RFCENVG) {
			printk(KERN_INFO "rfork with RFCENVG unimplemented!\n");
		}
		if (flags & RFNOTEG) {
			printk(KERN_INFO "rfork with RNOTEG unimplemented!\n");
		}
		if (flags & RFREND) {
			printk(KERN_INFO "rfork with RFREND unimplemented!\n");
		}
		if (flags & RFMEM) {
			printk(KERN_INFO "rfork with RFCENVG unimplemented!\n");
		}

		regs.bx = clone_flags;
		ret = sys_clone(regs);

		if (flags & RFCNAMEG) {
			printk(KERN_INFO "rfork with RFCNAMEG called, unsharing!\n");
			sys_unshare(CLONE_NEWNS);
		}
		if (flags & RFFDG) {
			printk(KERN_INFO "rfork with RFFDG unimplemented!\n");
		} else if (flags & RFCFDG) {
			printk(KERN_INFO "rfork with RFCFDG called, unsharing!\n");
			sys_unshare(CLONE_FILES);
		}
	}
	
	return ret;
}

