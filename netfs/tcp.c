/*
 * Copyright 2009 Rahul Murmuria <rahul@murmuria.in>
 * This file may be redistributed under the terms of the GNU GPL.
 */

#define __NO_VERSION__

#include "net.h"
/*
 * Create the files that we export.
 */

void tcp_create_files (struct super_block *sb, struct dentry *root)
{
 	static char *clone_tmp, *stats_tmp;
	struct dentry *subdir;
	
	subdir = slashnet_create_dir(sb, root, "tcp");
	if (subdir) {
		clone_tmp = kmalloc(TMPSIZE, GFP_KERNEL);
		memset (clone_tmp, 0, TMPSIZE);
		slashnet_create_file(sb, subdir, "clone", clone_tmp);
		kfree(clone_tmp);

		stats_tmp = kmalloc(TMPSIZE, GFP_KERNEL);
		memset (stats_tmp, 0, TMPSIZE);
		slashnet_create_file(sb, subdir, "stats", stats_tmp);
		kfree(stats_tmp);
	}
}



