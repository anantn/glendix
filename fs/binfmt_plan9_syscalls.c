/*
 * Copyright (C) 2008 Anant Narayanan <anant@kix.in>
 * Plan 9 system call implementations
 */

#include <linux/fs.h>
#include <linux/syscalls.h>

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

asmlinkage long sys_plan9_close(struct pt_regs regs)
{
	unsigned long arg1;
	unsigned long *addr = (unsigned long *)regs.esp;
	printk(KERN_INFO "P9: Syscall %ld close called!\n", regs.eax);

	get_user(arg1, ++addr);
	return sys_close(arg1);
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

