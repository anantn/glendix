/*
 * Copyright 2009 Rahul Murmuria <rahul@murmuria.in>
 * This file may be redistributed under the terms of the GNU GPL.
 */

#define __NO_VERSION__

#include "net.h"
/*
 * Create the files that we export.
 */

void cs_create_files (struct super_block *sb, struct dentry *root)
{
 	static char *cs_tmp;
	cs_tmp = kmalloc(TMPSIZE, GFP_KERNEL);
	memset (cs_tmp, 0, TMPSIZE);
	slashnet_create_file(sb, root, "cs", cs_tmp);
	kfree(cs_tmp);
}



