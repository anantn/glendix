/*
 * Copyright 2009 Rahul Murmuria <rahul@murmuria.in>
 * This file may be redistributed under the terms of the GNU GPL.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>     	/* This is where libfs stuff is declared */

#define NET_MAGIC 0x19980122

static inline unsigned int blksize_bits(unsigned int size)
{
    unsigned int bits = 8;
    do {
        bits++;
        size >>= 1;
    } while (size > 256);
    return bits;
}

#define TMPSIZE 128

static struct inode *slashnet_make_inode(struct super_block *, int);


/*
 * The operations on our "files".
 */

static int slashnet_open(struct inode *, struct file *);
static ssize_t slashnet_read_file(struct file *, char *,
		size_t, loff_t *);
static ssize_t slashnet_write_file(struct file *, const char *,
		size_t, loff_t *);

/*
 * Create a file.
 */
static struct dentry *slashnet_create_file (struct super_block *,
		struct dentry *, const char *, char *);

/*
 * Create a directory
 */
static struct dentry *slashnet_create_dir (struct super_block *,
		struct dentry *, const char *);

/*
 * Create the files that we export.
 */

static void slashnet_create_files (struct super_block *, struct dentry *);
static void cs_create_files (struct super_block *, struct dentry *);




