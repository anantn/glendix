/*
 * Copyright 2009 Rahul Murmuria <rahul@murmuria.in>
 * This file may be redistributed under the terms of the GNU GPL.
 */

#define __NO_VERSION__

#include "net.h"
/*
 * Create the files that we export.
 */

void ether_create_files (struct super_block *sb, struct dentry *root)
{
 	static char *ether_tmp;
	ether_tmp = kmalloc(TMPSIZE, GFP_KERNEL);
	memset (ether_tmp, 0, TMPSIZE);
	/* if ether0 exists */
	slashnet_create_dir(sb, root, "ether0");
	kfree(ether_tmp);
}



