/*
    Purpose: 
    Author: Reece W.
    License: All Rights Reserved J. Reece Wilson
*/

#include "common.h"

#include <linux/string.h>
#include <linux/vmalloc.h>
#include <linux/kthread.h>
#include <linux/kallsyms.h>
#include <asm/fpu/api.h>
#include <linux/delay.h>

#include "file_helper.h"
#include "pe_headers.h"
#include "bootstrapper.h"
#include "shutdown.h"

struct xenus_mutex
{
    struct mutex mutex;
    atomic_t users;
};

mutex_k threading_create_mutex (void)
{
    struct xenus_mutex * xmutex = (struct xenus_mutex *) kmalloc(sizeof(struct xenus_mutex),GFP_ATOMIC);
    
    if (!xmutex)
        return NULL;
    
    atomic_set(&xmutex->users, 0);
    mutex_init(&xmutex->mutex);
    
    return (mutex_k) xmutex;
}

// fun linux bug that nobody can be arsed to fix
// https://lwn.net/Articles/575477/
// https://lwn.net/Articles/575460/
// my favourite kind of bug
//
// how ironic
// mutexes, the overseers of atomic deallocation operations, don't reference count themselves allowing for self-dellocation in the middle of an unlock operation
// tl;dr a very slow thread could be in mutex_unlock after mutex_destroy has been called from the unlocker of said thread 

void threading_mutex_destroy(mutex_k * mutex_)
{
    struct xenus_mutex * mutex = (struct xenus_mutex *)mutex_;
    
    while (atomic_read(&mutex->users))
        continue;
    
    mutex_destroy(&mutex->mutex);
    kfree(mutex);
}

void threading_mutex_unlock(mutex_k * mutex_)
{
    struct xenus_mutex * mutex = (struct xenus_mutex *)mutex_;
    
    atomic_add(1, &mutex->users);
    mutex_unlock(&mutex->mutex);
    atomic_sub(1, &mutex->users);
}

void init_mutex(bootstrap_t * functions)
{
    functions->mutex.mutex_init         = threading_create_mutex;
    functions->mutex.mutex_lock         = mutex_lock;
    functions->mutex.mutex_unlock       = threading_mutex_unlock;
    functions->mutex.mutex_trylock      = mutex_trylock;
    functions->mutex.mutex_destroy      = threading_mutex_destroy;
    functions->mutex.mutex_is_locked    = mutex_is_locked;
}

int ms_threading_cb(void *tdata)
{
    //UGLY! UGLY! TODO (Reece): Rewrite!
    void * data;
    size_t * buffer;
    thread_callback_t callback;
    
    buffer = (size_t *) tdata;
    callback = (thread_callback_t) buffer[0];
    data = (void *) buffer[1];
    
    kfree(tdata);
    return callback(data);
}

task_k threading_create_thread_unsafe(thread_callback_t callback, void * data, const char * name, bool run)
{
    char * buffer;
    
    if (!callback)
        return NULL;
    
    buffer = (char *) kmalloc(sizeof(size_t) * 2, GFP_ATOMIC);
    
    if (!buffer)
        return NULL;
    
    *(size_t *)(buffer + 0)              = (size_t) callback;
    *(size_t *)(buffer + sizeof(size_t)) = (size_t) data;
    
    if (run)
        return (task_k) kthread_run(ms_threading_cb, buffer, name);
    return (task_k) kthread_create(ms_threading_cb, buffer, name);
}

uint64_t threading_get_pid(void)
{
    return current->pid;
}

task_k threading__current(void)
{
    return (task_k) current;
}

thread_storage_data_p  threading_tls(void)
{
    return (thread_storage_data_p) &(current->xenus);
}

void threading_preempt_enable(void)
{
    barrier();
    preempt_count_dec_and_test();
}

void threading_preempt_enable_reseched(void)
{
    barrier();
    if (unlikely(preempt_count_dec_and_test()))
        __preempt_schedule();
}

void threading_preempt_disable(void)
{
    preempt_disable();
}

void threading_kernel_fpu_begin(void)
{
    //printk("[CALL TO] kernel_fpu_begin\n");
    kernel_fpu_begin();
}

void threading_kernel_fpu_end(void)
{
    //printk("[CALL TO] kernel_fpu_end\n");
    kernel_fpu_end();
}

void threading___kernel_fpu_begin(void)
{
    //printk("[CALL TO] kernel_fpu_begin\n");
    __kernel_fpu_begin();
}

void threading___kernel_fpu_end(void)
{
    //printk("[CALL TO] kernel_fpu_end\n");
    __kernel_fpu_end();
}

void init_cpu(bootstrap_t * functions)
{
    functions->cpu.kernel_fpu_begin         = threading_kernel_fpu_begin;
    functions->cpu.kernel_fpu_end           = threading_kernel_fpu_end;
    functions->cpu.__kernel_fpu_begin       = threading___kernel_fpu_begin;
    functions->cpu.__kernel_fpu_end         = threading___kernel_fpu_end;
    functions->cpu.create_thread_unsafe     = threading_create_thread_unsafe;
    functions->cpu.get_current_pid          = threading_get_pid;
    functions->cpu._current                 = threading__current;
    functions->cpu.tls                      = threading_tls;
    functions->cpu.preempt_enable           = threading_preempt_enable;
    functions->cpu.preempt_enable_reseched  = threading_preempt_enable_reseched;
    functions->cpu.preempt_disable          = threading_preempt_disable;
}

void *  mem_execalloc(size_t size)
{
    return __vmalloc(size, GFP_KERNEL, PAGE_KERNEL_EXEC);
}

uint64_t  linux_page_to_pfn(void * aa)
{
    return page_to_pfn((struct page *)aa);
}

void * linux_pfn_to_page(uint64_t pfn)
{
    return pfn_to_page(pfn);
}

void init_memory(bootstrap_t * functions)
{                                           
    functions->memory.execalloc             = mem_execalloc;  
    functions->memory.execfree              = vfree;   
    functions->memory.kmalloc               = kmalloc;   
    functions->memory.vmalloc               = vmalloc; 
    functions->memory.kfree                 = kfree;     
    functions->memory.vfree                 = vfree;    
    functions->memory.ksize                 = ksize;    
    functions->memory.memcpy                = memcpy;      
    functions->memory.memset                = memset;     
    functions->memory.memmove               = memmove;    
    functions->memory.memcmp                = memcmp;     
    functions->memory.memscan               = memscan;  
    functions->memory.linux_page_to_pfn     = linux_page_to_pfn;  
    functions->memory.linux_pfn_to_page     = linux_pfn_to_page;  
    
}

void init_strings(bootstrap_t * functions)
{    
    functions->string.strcasecmp    = strcasecmp;       
    functions->string.strncasecmp   = strncasecmp;      
    functions->string.strncpy       = strncpy;          
    functions->string.strlcpy       = strlcpy;          
    functions->string.strcat        = strcat;           
    functions->string.strncat       = strncat;          
    functions->string.strlcat       = strlcat;          
    functions->string.strcmp        = strcmp;           
    functions->string.strncmp       = strncmp;          
    functions->string.strchr        = strchr;           
    functions->string.strrchr       = strrchr;          
    functions->string.strnchr       = strnchr;          
    functions->string.skip_spaces   = skip_spaces;      
    functions->string.strim         = strim;            
    functions->string.strlen        = strlen;           
    functions->string.strnlen       = strnlen;          
    functions->string.strspn        = strspn;           
    functions->string.strcspn       = strcspn;          
    functions->string.strpbrk       = strpbrk;          
    functions->string.strsep        = strsep;           
    functions->string.sysfs_streq   = sysfs_streq;      
    functions->string.strtobool     = strtobool;        
    functions->string.strstr        = strstr;           
    functions->string.strnstr       = strnstr;          
    functions->string.strcpy        = strcpy;           
}

int dbg_print(const char * msg)
{
    return printk("%s", msg); //puts isn't a symbol?
}

void dbg_panic(const char * msg)
{
#ifdef CONFIG_PANIC_DBG
    printk("%s\n", msg); //puts isn't a symbol?
    while (1)
        ssleep(2000);
#else
    panic(msg);
#endif
}

void test_function(size_t a_1, size_t a_2, size_t a_3, size_t a_4, size_t a_5, size_t a_6, size_t a_7, size_t a_8, size_t a_9, size_t a_10, size_t a_11, size_t a_12)
{
    if (a_1 == 69)
        ((void *(*)(size_t, size_t, size_t, size_t, size_t, size_t, size_t, size_t, size_t, size_t, size_t, size_t, size_t, size_t, size_t))a_2)(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
    else
        printk("Microsoft to SystemV test (%i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i) \n", a_1, a_2, a_3, a_4, a_5, a_6, a_7, a_8, a_9, a_10, a_11, a_12);
}

void dbg_on_reload(void(XENUS_MS_ABI * callback)(void))
{
    shutdown_set_handler(callback);
}

void init_dbg(bootstrap_t * functions)
{
    functions->dbg.test_function = test_function;
    functions->dbg.panic         = dbg_panic; //= panic;
    functions->dbg.print         = dbg_print;
    functions->dbg.reload        = dbg_on_reload;
}

void init_symbols(bootstrap_t * functions)
{
    functions->symbols.kallsyms_lookup_name = kallsyms_lookup_name;
    functions->symbols.__symbol_get         = __symbol_get;
}

void init_fio_helper(bootstrap_t * functions)
{
    functions->fio.file_open              = file_open;
    functions->fio.file_open_readonly     = file_open_readonly;
    functions->fio.file_open_readwrite    = file_open_readwrite;
    functions->fio.file_open_append       = file_open_append;
    functions->fio.file_read              = file_read;
    functions->fio.file_write             = file_write;
    functions->fio.file_mode              = file_mode;
    functions->fio.file_length            = file_length;
    functions->fio.file_mt                = file_mt;
    functions->fio.file_ct                = file_ct;
    functions->fio.file_at                = file_at;
    functions->fio.file_sync              = file_sync;
    functions->fio.file_close             = file_close;
}

void bootstrap_functions(bootstrap_t * bootstrap)
{
    memset(bootstrap, 0, sizeof(bootstrap_t));
    
    init_dbg(bootstrap);
    init_strings(bootstrap);
    init_memory(bootstrap);
    init_cpu(bootstrap);
    init_mutex(bootstrap);
    init_symbols(bootstrap);
    init_fio_helper(bootstrap);
}