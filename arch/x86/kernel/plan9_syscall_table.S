ENTRY(plan9_syscall_table)
	.long sys_plan9_unimplemented /* 0 */
	.long sys_plan9_deprecated    /* _errstr */
	.long sys_plan9_unimplemented
	.long sys_plan9_unimplemented
	.long sys_plan9_close
	.long sys_plan9_unimplemented /* 5 */
	.long sys_plan9_unimplemented
	.long sys_plan9_unimplemented
	.long sys_plan9_exits
	.long sys_plan9_deprecated    /* _fsession */
	.long sys_plan9_unimplemented /* 10 */
	.long sys_plan9_deprecated    /* _fstat */
	.long sys_plan9_unimplemented
	.long sys_plan9_deprecated    /* _mount */
	.long sys_plan9_open
	.long sys_plan9_deprecated    /* 15, _read */
	.long sys_plan9_unimplemented
	.long sys_plan9_sleep
	.long sys_plan9_deprecated    /* _stat */
	.long sys_plan9_unimplemented
	.long sys_plan9_deprecated    /* 20, _write */
	.long sys_plan9_unimplemented
	.long sys_plan9_create
	.long sys_plan9_fd2path
	.long sys_plan9_unimplemented
	.long sys_plan9_remove		  /* 25 */
	.long sys_plan9_deprecated    /* _wstat */
	.long sys_plan9_deprecated    /* _fwstat */
	.long sys_plan9_unimplemented
	.long sys_plan9_unimplemented
	.long sys_plan9_unimplemented /* 30 */
	.long sys_plan9_unimplemented
	.long sys_plan9_unimplemented
	.long sys_plan9_unimplemented
	.long sys_plan9_unimplemented
	.long sys_plan9_unimplemented /* 35 */
	.long sys_plan9_deprecated    /* _wait */
	.long sys_plan9_unimplemented
	.long sys_plan9_unimplemented
	.long sys_plan9_seek
	.long sys_plan9_unimplemented /* 40 */
	.long sys_plan9_unimplemented
	.long sys_plan9_unimplemented
	.long sys_plan9_unimplemented
	.long sys_plan9_unimplemented
	.long sys_plan9_unimplemented /* 45 */
	.long sys_plan9_unimplemented
	.long sys_plan9_unimplemented
	.long sys_plan9_unimplemented /* MISSING */
	.long sys_plan9_unimplemented /* MISSING */
	.long sys_plan9_pread         /* 50 */
	.long sys_plan9_pwrite
END(plan9_syscall_table)
