/*
 * Copyright 2009 Rahul Murmuria <rahul@murmuria.in>
 * This file may be redistributed under the terms of the GNU GPL.
 */

#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/pagemap.h> 	/* PAGE_CACHE_SIZE */
#include <linux/fs.h>     	/* This is where libfs stuff is declared */
#include "net.h"

/*
 * Create the files that we export.
 */

static void cs_create_files (struct super_block *sb, struct dentry *root)
{
 	static char *cs_tmp;
	cs_tmp = kmalloc(TMPSIZE, GFP_KERNEL);
	memset (cs_tmp, 0, TMPSIZE);
	slashnet_create_file(sb, root, "cs", cs_tmp);
	slashnet_create_dir(sb, root, "tcp");
	kfree(cs_tmp);
}



