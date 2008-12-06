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
