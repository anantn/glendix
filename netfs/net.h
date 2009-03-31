/*
 * Copyright 2009 Rahul Murmuria <rahul@murmuria.in>
 * This file may be redistributed under the terms of the GNU GPL.
 */
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/pagemap.h>

#include <asm/atomic.h>
#include <asm/uaccess.h>

#define NET_MAGIC 0x19980122
#define TMPSIZE 128

/*
 * Create a file.
 */
struct dentry *slashnet_create_file (struct super_block *,
		struct dentry *, const char *, char *);

/*
 * Create a directory
 */
struct dentry *slashnet_create_dir (struct super_block *,
		struct dentry *, const char *);

/*
 * Create the files that we export.
 */
void cs_create_files (struct super_block *, struct dentry *);

