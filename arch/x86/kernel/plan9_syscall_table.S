ENTRY(plan9_syscall_table)
	.long sys_plan9_sysr        /* 0 */
	.long sys_plan9_deprecated  /* _errstr */
	.long sys_plan9_bind
	.long sys_plan9_chdir
	.long sys_plan9_close
	.long sys_plan9_dup         /* 5 */
	.long sys_plan9_alarm
	.long sys_plan9_exec
	.long sys_plan9_exits
	.long sys_plan9_deprecated  /* _fsession */
	.long sys_plan9_fauth       /* 10 */
	.long sys_plan9_deprecated  /* _fstat */
	.long sys_plan9_segbrk
	.long sys_plan9_deprecated  /* _mount */
	.long sys_plan9_open
	.long sys_plan9_deprecated  /* 15, _read */
	.long sys_plan9_oseek
	.long sys_plan9_sleep
	.long sys_plan9_deprecated  /* _stat */
	.long sys_plan9_rfork
	.long sys_plan9_deprecated  /* 20, _write */
	.long sys_plan9_pipe
	.long sys_plan9_create
	.long sys_plan9_fd2path
	.long sys_plan9_brk
	.long sys_plan9_remove      /* 25 */
	.long sys_plan9_deprecated  /* _wstat */
	.long sys_plan9_deprecated  /* _fwstat */
	.long sys_plan9_notify
	.long sys_plan9_noted
	.long sys_plan9_segattach   /* 30 */
	.long sys_plan9_segdetach
	.long sys_plan9_segfree
	.long sys_plan9_segflush
	.long sys_plan9_rendezvous
	.long sys_plan9_unmount     /* 35 */
	.long sys_plan9_deprecated  /* _wait */
	.long sys_plan9_semacquire
	.long sys_plan9_semrelease
	.long sys_plan9_seek
	.long sys_plan9_fversion    /* 40 */
	.long sys_plan9_errstr
	.long sys_plan9_stat
	.long sys_plan9_fstat
	.long sys_plan9_wstat
	.long sys_plan9_fwstat      /* 45 */
	.long sys_plan9_mount
	.long sys_plan9_await
	.long sys_plan9_deprecated  /* MISSING */
	.long sys_plan9_deprecated  /* MISSING */
	.long sys_plan9_pread       /* 50 */
	.long sys_plan9_pwrite
END(plan9_syscall_table)