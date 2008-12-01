/*
 * Copyright (C) 2008 Anant Narayanan
 * Plan 9 system call implementations
 */

#include <linux/fs.h>
#include <linux/syscalls.h>

#include <asm/uaccess.h>
#include <asm/processor.h>

static void called(static char *name)
{
	printk(KERN_ALERT "P9: %s Called!\n", name);
}

asmlinkage long sys_plan9_deprecated(struct pt_regs regs)
{
	printk(KERN_ALERT "P9: Syscall number %ld DEPRECATED!\n", regs.ax);
	return 0;
}

asmlinkage long sys_plan9_exits(struct pt_regs regs)
{
	called("exits");
	return sys_exit(1);
}
