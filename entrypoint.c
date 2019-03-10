/*
    Purpose: 
    Author: Reece W.
    License: All Rights Reserved J. Reece Wilson
*/
#include "common.h"
#include "pe_loader.h"
#include "file_helper.h"
#include "bootstrapper.h"
#include "os_struct.h" 
#include "shutdown.h"

static void * ps_buffer = 0;
static uint32_t ps_length = 0;

static void * pe_base;
static void * pe_entrypoint;

static bool test_kern_types(void)
{
#define TEST_TYPE(type, bytes) if (sizeof(type) != bytes) return false;
    TEST_TYPE(short, 2)
    TEST_TYPE(int, 4)
    TEST_TYPE(long, 8)
    TEST_TYPE(long long, 8)
    TEST_TYPE(signed, 4)
    TEST_TYPE(unsigned, 4)
#undef TEST_TYPE
    return true;
}

static int init_portable_structs(void)
{
    ps_length = ps_buffer_length();
    ps_buffer = kmalloc(ps_length, GFP_KERNEL | GFP_ATOMIC);
    
    if (!ps_buffer) 
    {
        printk("Xenus OS ran out of memory \n");
        return 1;
    }
    
    ps_initialize();
    ps_buffer_dump(ps_buffer, ps_length);
    return 0;
}

static int init_pe(void)
{
    void * buffer  = 0;  
    void * file = 0;
    long length;
    
    file = file_open_readonly(BOOTSTRAP_DLL);
    
    if (!file)
    {
        printk(KERN_INFO "Xenus OS kernel not found!\n");
        return 1;
    }
    
    length = file_length(BOOTSTRAP_DLL);
    buffer = kmalloc(length, GFP_KERNEL | GFP_ATOMIC);
    
    if (file_read(file, 0, buffer, length) != length)
    {
        printk(KERN_INFO "Xenus OS kernel could not be read! length\n");
        return 1;
    }
    
    pe_base = load_pe(buffer, length, &pe_entrypoint);
    kfree(buffer);
    
    if (!pe_entrypoint)
    {
        printk(KERN_INFO "Xenus OS couldn't find the entrypoint. (why the fuck are we even checking? its not like we do any serious integrity checks anymore)\n");
        return 1;
    }
    
    printk("Loaded Xenus Kernel PE from %lli \n", file_ct(BOOTSTRAP_DLL));
    
    return 0;
}

static int __init bs_init(void)
{
    bootstrap_t functions;
    linux_info_t info;
    
    if (!test_kern_types()) return -210;
    if (init_portable_structs()) return -220;
    if (init_pe()) return -230;
    
    printk(KERN_INFO "Xenus starting up...\n");
    
    bootstrap_functions(&functions);
    init_os_struct(&info);
    
    return ((kernel_startpoint_t)pe_entrypoint)(pe_base, &functions, NULL, 0, ps_buffer, ps_length, &info);
}

static void __exit bs_exit(void){
    printk(KERN_INFO "Xenus OS shutting down!\n");
    shutdown_trigger();
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Reece Wilson");
MODULE_DESCRIPTION("Xenus kernel boot[loader/strap] as Linux Kernel module");
MODULE_VERSION("-1");

module_init(bs_init);
module_exit(bs_exit);
