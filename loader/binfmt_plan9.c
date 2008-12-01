/*
 * Binary loader for Plan 9's a.out executable format
 * 
 * Copyright (C) 2008 Anant Narayanan
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/ptrace.h>
#include <linux/binfmts.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/mman.h>
#include <linux/personality.h>

#include <asm/page.h>
#include <asm/uaccess.h>
#include <asm/processor.h>
#include <asm/byteorder.h>

#include "binfmt_plan9.h"

static int load_plan9_binary(struct linux_binprm *, struct pt_regs *);

static struct linux_binfmt plan9_format = {
	.module = THIS_MODULE,
	.load_binary = load_plan9_binary,
	.load_shlib = NULL,
	.core_dump = NULL
};

void print_mems(void)
{
	int i = 1;
	struct vm_area_struct *blah = current->mm->mmap;
	while (blah != NULL) {
		printk(KERN_ALERT "9load: Range %d: %lx to %lx\n", i++,
		       blah->vm_start, blah->vm_end);
		blah = blah->vm_next;
	}
}

static unsigned long __user *create_args(char __user * p,
					 struct linux_binprm *bprm,
					 struct pt_regs *regs)
{
	char __user *__user * argv;
	unsigned long __user *sp;
	int argc = bprm->argc;

	sp = (void __user *)
		((-(unsigned long)sizeof(char *)) & (unsigned long)p);

	/* leave space for TOS */
	sp -= TOS_SIZE;
	regs->bx = (unsigned long)sp;

	sp -= argc + 1;
	argv = (char __user * __user *)sp;

	put_user(argc, --sp);

	current->mm->arg_start = (unsigned long)p;
	while (argc-- > 0) {
		char c;
		put_user(p, argv++);
		do {
			get_user(c, p++);
		} while (c);
	}
	put_user(NULL, argv);
	current->mm->arg_end = current->mm->env_start = current->mm->env_end =
		(unsigned long)p;

	return sp;
}

static int load_plan9_binary(struct linux_binprm *bprm, struct pt_regs *regs)
{
	struct plan9_exec ex;
	unsigned long rlim, retval, error, fpos = 0, tot = 0;
	loff_t pos;

	/* Load header and fix big-endianess */
	ex = *((struct plan9_exec *)bprm->buf);
	ex.magic = be32_to_cpu((__u32)ex.magic);
	ex.text = be32_to_cpu((__u32)ex.text);
	ex.data = be32_to_cpu((__u32)ex.data);
	ex.bss = be32_to_cpu((__u32)ex.bss);
	ex.syms = be32_to_cpu((__u32)ex.syms);
	ex.entry = be32_to_cpu((__u32)ex.entry);
	ex.spsz = be32_to_cpu((__u32)ex.spsz);
	ex.pcsz = be32_to_cpu((__u32)ex.pcsz);

	tot = 0x20 + ex.text + ex.data + ex.syms + ex.spsz + ex.pcsz;

	/* Check if this is really a plan 9 executable */
	if (ex.magic != I_MAGIC)
		return -ENOEXEC;
	
	printk(KERN_ALERT "9load: %lx %lx %lx %lx %lx %lx\n",
		ex.magic, ex.text, ex.data, ex.bss, ex.syms, ex.entry);

	/* Check initial limits. This avoids letting people circumvent
	 * size limits imposed on them by creating programs with large
	 * arrays in the data or bss.
	 */
	rlim = current->signal->rlim[RLIMIT_DATA].rlim_cur;
	if (rlim >= RLIM_INFINITY)
		rlim = ~0;
	if (ex.data + ex.bss > rlim)
		return -ENOMEM;

	/* Flush all traces of the currently running executable */
	retval = flush_old_exec(bprm);
	if (retval) {
		printk(KERN_ALERT "9load: flush failed! %lx\n", retval);
		return retval;
	}
	/* Point of no return */
	set_personality(PER_LINUX);

	/* Set code sections */
	current->mm->end_code = TXT_ADDR +
	    (current->mm->start_code = STR_ADDR);
	current->mm->end_data = ex.data +
	    (current->mm->start_data = PAGE_ALIGN(current->mm->end_code));
	current->mm->brk = ex.bss +
	    (current->mm->start_brk = current->mm->end_data);
	current->mm->mmap_base = 0;
	current->mm->free_area_cache = 0;
	current->mm->cached_hole_size = 0;

	current->flags &= ~PF_FORKNOEXEC;

	printk(KERN_ALERT "9load: %lx %lx %lx %lx %lx %lx %lx %lx\n",
	       current->mm->start_code,
	       current->mm->end_code,
	       current->mm->start_data,
	       current->mm->end_data,
	       current->mm->start_brk,
	       current->mm->brk, current->mm->mmap_base, tot);

	/* mmap text in */
	down_write(&current->mm->mmap_sem);
	fpos = do_mmap(bprm->file, STR_ADDR, TXT_ADDR,
		       PROT_READ | PROT_EXEC,
		       MAP_FIXED | MAP_PRIVATE | MAP_EXECUTABLE, 0);
	up_write(&current->mm->mmap_sem);

	/* copy data in */
	down_write(&current->mm->mmap_sem);
	error =
	    do_mmap(NULL, DAT_ADDR, ex.data + ex.bss,
		    PROT_READ | PROT_WRITE, MAP_FIXED | MAP_PRIVATE, 0);
	up_write(&current->mm->mmap_sem);

	pos = TXT_ADDR;
	bprm->file->f_op->read(bprm->file,
			       (char *)DAT_ADDR,
			       ex.data + ex.bss, &pos);

	set_binfmt(&plan9_format);
	retval = setup_arg_pages(bprm, TASK_SIZE, EXSTACK_DEFAULT);
	if (retval < 0) {
		send_sig(SIGKILL, current, 0);
		return retval;
	}

	printk(KERN_ALERT "9load: BPRM Value: %lx\n", bprm->p);

	current->mm->start_stack =
	    (unsigned long)create_args((char __user *)bprm->p, bprm, regs);
	printk(KERN_ALERT "9load: Stack start: %lx, TOS: %lx\n",
	       current->mm->start_stack, regs->bx);

	print_mems();

	start_thread(regs, ex.entry, current->mm->start_stack);
	printk(KERN_ALERT "9load: Program started: EBX: %lx, EIP: %lx\n",
	       regs->bx, regs->ip);

	return 0;
}

static int __init plan9_init(void)
{
	printk(KERN_ALERT "Hello, Plan9!\n");
	return register_binfmt(&plan9_format);
}

static void __exit plan9_exit(void)
{
	unregister_binfmt(&plan9_format);
	printk(KERN_ALERT "Goodbye, Plan9!\n");
}

module_init(plan9_init);
module_exit(plan9_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anant Narayanan <anant@kix.in>");
MODULE_DESCRIPTION("Binary loader for Plan9's a.out executable format");
