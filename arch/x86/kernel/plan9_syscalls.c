/*
 * Copyright (C) 2008 Anant Narayanan
 * Plan 9 system call implementations
 */

#ifdef CONFIG_BINFMT_PLAN9

#include <linux/fs.h>
#include <linux/syscalls.h>

#include <asm/uaccess.h>
#include <asm/processor.h>

static void unimplemented(int num)
{
	printk(KERN_ALERT "P9: %d called, but unimplemented!\n", num);
}

asmlinkage long sys_plan9_deprecated(struct pt_regs regs)
{
	printk(KERN_ALERT "P9: syscall number %ld DEPRECATED!\n", regs.ax);
	return 0;
}

asmlinkage long sys_plan9_sysr(struct pt_regs regs)
{
	unimplemented(regs.ax); return 0;
}

asmlinkage long sys_plan9_bind(struct pt_regs regs)
{
	unimplemented(regs.ax); return 0;
}

asmlinkage long sys_plan9_chdir(struct pt_regs regs)
{
	unimplemented(regs.ax); return 0;
}

asmlinkage long sys_plan9_close(struct pt_regs regs)
{
	unimplemented(regs.ax); return 0;
}

asmlinkage long sys_plan9_dup(struct pt_regs regs)
{
	unimplemented(regs.ax); return 0;
}

asmlinkage long sys_plan9_alarm(struct pt_regs regs)
{
	unimplemented(regs.ax); return 0;
}

asmlinkage long sys_plan9_exec(struct pt_regs regs)
{
	unimplemented(regs.ax); return 0;
}

asmlinkage long sys_plan9_exits(struct pt_regs regs)
{
	return sys_exit(1);
}

asmlinkage long sys_plan9_fauth(struct pt_regs regs)
{
	unimplemented(regs.ax); return 0;
}

asmlinkage long sys_plan9_segbrk(struct pt_regs regs)
{
	unimplemented(regs.ax); return 0;
}

asmlinkage long sys_plan9_open(struct pt_regs regs)
{
	unimplemented(regs.ax); return 0;
}

asmlinkage long sys_plan9_oseek(struct pt_regs regs)
{
	unimplemented(regs.ax); return 0;
}

asmlinkage long sys_plan9_sleep(struct pt_regs regs)
{
	unimplemented(regs.ax); return 0;
}

asmlinkage long sys_plan9_rfork(struct pt_regs regs)
{
	unimplemented(regs.ax); return 0;
}

asmlinkage long sys_plan9_pipe(struct pt_regs regs)
{
	unimplemented(regs.ax); return 0;
}

asmlinkage long sys_plan9_create(struct pt_regs regs)
{
	unimplemented(regs.ax); return 0;
}

asmlinkage long sys_plan9_fd2path(struct pt_regs regs)
{
	unimplemented(regs.ax); return 0;
}

asmlinkage long sys_plan9_brk(struct pt_regs regs)
{
	unimplemented(regs.ax); return 0;
}

asmlinkage long sys_plan9_remove(struct pt_regs regs)
{
	unimplemented(regs.ax); return 0;
}

asmlinkage long sys_plan9_notify(struct pt_regs regs)
{
	unimplemented(regs.ax); return 0;
}

asmlinkage long sys_plan9_noted(struct pt_regs regs)
{
	unimplemented(regs.ax); return 0;
}

asmlinkage long sys_plan9_segattach(struct pt_regs regs)
{
	unimplemented(regs.ax); return 0;
}

asmlinkage long sys_plan9_segdetach(struct pt_regs regs)
{
	unimplemented(regs.ax); return 0;
}

asmlinkage long sys_plan9_segfree(struct pt_regs regs)
{
	unimplemented(regs.ax); return 0;
}

asmlinkage long sys_plan9_segflush(struct pt_regs regs)
{
	unimplemented(regs.ax); return 0;
}

asmlinkage long sys_plan9_rendezvous(struct pt_regs regs)
{
	unimplemented(regs.ax); return 0;
}

asmlinkage long sys_plan9_unmount(struct pt_regs regs)
{
	unimplemented(regs.ax); return 0;
}

asmlinkage long sys_plan9_semacquire(struct pt_regs regs)
{
	unimplemented(regs.ax); return 0;
}

asmlinkage long sys_plan9_semrelease(struct pt_regs regs)
{
	unimplemented(regs.ax); return 0;
}

asmlinkage long sys_plan9_seek(struct pt_regs regs)
{
	unimplemented(regs.ax); return 0;
}

asmlinkage long sys_plan9_fversion(struct pt_regs regs)
{
	unimplemented(regs.ax); return 0;
}

asmlinkage long sys_plan9_errstr(struct pt_regs regs)
{
	unimplemented(regs.ax); return 0;
}

asmlinkage long sys_plan9_stat(struct pt_regs regs)
{
	unimplemented(regs.ax); return 0;
}

asmlinkage long sys_plan9_fstat(struct pt_regs regs)
{
	unimplemented(regs.ax); return 0;
}

asmlinkage long sys_plan9_wstat(struct pt_regs regs)
{
	unimplemented(regs.ax); return 0;
}

asmlinkage long sys_plan9_fwstat(struct pt_regs regs)
{
	unimplemented(regs.ax); return 0;
}

asmlinkage long sys_plan9_mount(struct pt_regs regs)
{
	unimplemented(regs.ax); return 0;
}

asmlinkage long sys_plan9_await(struct pt_regs regs)
{
	unimplemented(regs.ax); return 0;
}

asmlinkage long sys_plan9_pread(struct pt_regs regs)
{
	unimplemented(regs.ax); return 0;
}

asmlinkage long sys_plan9_pwrite(struct pt_regs regs)
{
	unimplemented(regs.ax); return 0;
}

#endif
