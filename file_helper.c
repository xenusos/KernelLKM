/*
    Purpose: File IO helper functions
    Author: Reece W.
    License: All Rights Reserved J. Reece Wilson
*/
#include "common.h"

#include <linux/fs.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/mm.h> 
#include <linux/buffer_head.h>

#include "file_helper.h"

unsigned long long to_milliseconds(struct timespec tv)
{
    return (tv.tv_sec * 1000) + (tv.tv_nsec / 1000000);
}

void * file_open_readonly(const char *path) 
{
    return file_open(path, O_RDONLY, 0);
}

void * file_open_readwrite(const char *path) 
{
    return file_open(path, O_RDWR, 0644);
}

void * file_open_append(const char *path) 
{
    return file_open(path, O_RDWR|O_APPEND, 0644);
}

void * file_open(const char *path, int flags, int rights) 
{
    int err = 0;
    struct file *filp = NULL;
	
    filp = filp_open(path, flags, rights);

    if (IS_ERR(filp)) {
        err = PTR_ERR(filp);
        return NULL;
    }
    
    return (void *) filp;
}

long file_length(const char *path)
{
    struct kstat stat;
    struct file * f;

    if (IS_ERR(f = filp_open(path, O_RDONLY, 0)))
        return -1;
	
    if (!(vfs_getattr(&f->f_path, &stat, STATX_ALL, KSTAT_QUERY_FLAGS)))
    {
        filp_close(f, 0);
        return -1;
    }
	
    filp_close(f, 0);
    
    return stat.size;
}

long  file_mode(const char * path)
{
    struct kstat stat;
    struct file * f;

    if (IS_ERR(f = filp_open(path, O_RDONLY, 0)))
        return -1;
	
    if (!(vfs_getattr(&f->f_path, &stat, STATX_ALL, KSTAT_QUERY_FLAGS)))
    {
        filp_close(f, 0);
        return -1;
    }
	
    filp_close(f, 0);
    
    return stat.mode;
}

unsigned long long  file_ct(const char * path)
{
    struct kstat stat;
    struct file * f;

    if (IS_ERR(f = filp_open(path, O_RDONLY, 0)))
        return -1;
	
    if (!(vfs_getattr(&f->f_path, &stat, STATX_ALL, KSTAT_QUERY_FLAGS)))
    {
        filp_close(f, 0);
        return -1;
    }
	
    filp_close(f, 0);
    
    return to_milliseconds(stat.ctime);
}

unsigned long long  file_mt(const char * path)
{
    struct kstat stat;
    struct file * f;

    if (IS_ERR(f = filp_open(path, O_RDONLY, 0)))
        return -1;
	
    if (!(vfs_getattr(&f->f_path, &stat, STATX_ALL, KSTAT_QUERY_FLAGS)))
    {
        filp_close(f, 0);
        return -1;
    }
	
    filp_close(f, 0);
    
    return to_milliseconds(stat.mtime);
}

unsigned long long  file_at(const char * path)
{
    struct kstat stat;
    struct file * f;

    if (IS_ERR(f = filp_open(path, O_RDONLY, 0)))
        return -1;
	
    if (!(vfs_getattr(&f->f_path, &stat, STATX_ALL, KSTAT_QUERY_FLAGS)))
    {
        filp_close(f, 0);
        return -1;
    }
	
    filp_close(f, 0);
		
    return to_milliseconds(stat.atime);
}

int  file_read(void *file_, unsigned long long offset, unsigned char *data, size_t size) 
{
    return kernel_read((struct file *) file_, data, size, &offset);
}  

int  file_write(void * file_, unsigned long long offset, unsigned char *data, size_t size) 
{
    return kernel_write((struct file *) file_, data, size, &offset);
}

int  file_sync(void *file) 
{
    vfs_fsync((struct file *) file, 0);
    return 0;
}

int  file_close(void *file) 
{
    return filp_close((struct file *)file, NULL);
}
