/*
 * Binary loader for Plan 9's a.out executable format
 * 
 * Copyright (C) 2008 Anant Narayanan <anant@kix.in>
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

/*
 * All Plan 9 programs linked with libc obtain the address of the
 * '_tos' structure from EAX when executing _main().
 *
 * In Linux, however, the value of EAX is not preserved through the exec() 
 * process, hence we look for the MOV(addr, EAX) instruction and mangle it
 * to MOV(addr, EBX) instead. (Note that we store the _tos address in EBX 
 * in create_args)
 */
static void mangle_tos(unsigned long entry)
{
	unsigned char a, b, c, d, e;

        /*
         * Error checking for get_user/put_user is not done because
         * the user-space isn't actually running yet
         */
	get_user(a, (char __user *)entry);
	get_user(b, (char __user *)entry + 1);
	get_user(c, (char __user *)entry + 2);
	get_user(d, (char __user *)entry + 3);
	get_user(e, (char __user *)entry + 4);

        /* Check if our MOV instruction is present */
	if (a == 0x83 && b == 0xEC && c == 0x48 && d == 0x89 && e == 0x05) {
	        /* Yes, so we change 0x05 (EAX) to 0x1D (EBX)
	         * (ref: Intel x86 software developer's manual, volume 2A)
	         */
		put_user(0x1D, (char __user *)entry + 4);
	}
}

/*
 * Setup the environment and argument variables on the user-space stack
 */
static unsigned long __user *create_args(char __user *p,
					 struct linux_binprm *bprm,
					 struct pt_regs *regs)
{
	char __user *__user * argv;
	unsigned long __user *sp;
	int argc = bprm->argc;
        
        unsigned long q = (unsigned long)p;
        
	sp = (void __user *)
	    ((-(unsigned long)sizeof(char *)) & q);

	/* leave space for TOS, and store address in EBX */
	sp -= TOS_SIZE;
	regs->bx = (unsigned long)sp;

	sp -= argc + 1;
	argv = (char __user * __user *)sp;

	put_user(argc, --sp);

	current->mm->arg_start = q;
	while (argc-- > 0) {
		char c;
		put_user(p, argv++);
		do {
			get_user(c, p++);
		} while (c);
	}
	put_user(NULL, argv);
	current->mm->arg_end = current->mm->env_start =
		current->mm->env_end = q;

	return sp;
}

static int load_plan9_binary(struct linux_binprm *bprm, struct pt_regs *regs)
{
	struct plan9_exec ex;
	unsigned long rlim, retval, error, fpos = 0, tot = 0;
	loff_t pos;

	/* Load header and fix big-endianess */
	ex = *((struct plan9_exec *)bprm->buf);
	ex.magic = be32_to_cpu(ex.magic);
	ex.text = be32_to_cpu(ex.text);
	ex.data = be32_to_cpu(ex.data);
	ex.bss = be32_to_cpu(ex.bss);
	ex.syms = be32_to_cpu(ex.syms);
	ex.entry = be32_to_cpu(ex.entry);
	ex.spsz = be32_to_cpu(ex.spsz);
	ex.pcsz = be32_to_cpu(ex.pcsz);

	tot = 0x20 + ex.text + ex.data + ex.syms + ex.spsz + ex.pcsz;

	/* Check if this is really a plan 9 executable */
	if (ex.magic != I_MAGIC)
		return -ENOEXEC;

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
		return retval;
	}
	/* Point of no return */
	set_personality(PER_LINUX);

	/* Set code sections */
	current->mm->end_code = TXT_ADDR(ex) +
		(current->mm->start_code = STR_ADDR);
	current->mm->end_data = ex.data +
		(current->mm->start_data = PAGE_ALIGN(current->mm->end_code));
	current->mm->brk = ex.bss +
		(current->mm->start_brk = current->mm->end_data);
	current->mm->mmap_base = 0;
	current->mm->free_area_cache = 0;
	current->mm->cached_hole_size = 0;

	current->flags &= ~PF_FORKNOEXEC;

	/* mmap text in */
	down_write(&current->mm->mmap_sem);
	fpos = do_mmap(bprm->file, STR_ADDR, TXT_ADDR(ex),
			PROT_READ | PROT_EXEC,
			MAP_FIXED | MAP_PRIVATE | MAP_EXECUTABLE, 0);
	up_write(&current->mm->mmap_sem);

	/* copy data in */
	down_write(&current->mm->mmap_sem);
	error = do_mmap(NULL, DAT_ADDR(ex), ex.data + ex.bss,
			PROT_READ | PROT_WRITE,
			MAP_FIXED | MAP_PRIVATE, 0);
	up_write(&current->mm->mmap_sem);

	pos = TXT_ADDR(ex);
	bprm->file->f_op->read(bprm->file,
				(char *)DAT_ADDR(ex), ex.data + ex.bss, &pos);

	/* setup env and arguments on stack */
	set_binfmt(&plan9_format);
	retval = setup_arg_pages(bprm, TASK_SIZE, EXSTACK_DEFAULT);
	if (retval < 0) {
		send_sig(SIGKILL, current, 0);
		return retval;
	}

	current->mm->start_stack =
		(unsigned long)create_args((char __user *)bprm->p,
						bprm, regs);

	mangle_tos(ex.entry);
	start_thread(regs, ex.entry, current->mm->start_stack);
	
	if (printk_ratelimit())
		printk(KERN_INFO "9load: Program started: "
			"EBX: %lx, EIP: %lx\n", regs->bx, regs->ip);

	return 0;
}

static int __init plan9_aout_init(void)
{
	return register_binfmt(&plan9_format);
}

static void __exit plan9_aout_exit(void)
{
	unregister_binfmt(&plan9_format);
}

core_initcall(plan9_aout_init);
module_exit(plan9_aout_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anant Narayanan <anant@kix.in>");
MODULE_DESCRIPTION("Binary loader for Plan9's a.out executable format");
