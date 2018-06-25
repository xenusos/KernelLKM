/*
    Purpose: 
    Author: Reece W.
    License: All Rights Reserved J. Reece Wilson
*/

#define BOOTSTRAP
#define XENUS_BUILD

#include <linux/init.h>   
#include <linux/module.h> 
#include <linux/kernel.h> 
#include <linux/slab.h>   
#include <linux/types.h>
#include <linux/sched.h>

#include <xenus.h>                // generic xenus types 
#include <_/_generic_bootstrap.h> // static functions to provide to the kernel
#include <x_linux.h>              // i fucked something up in kernel.xdll so this now separate