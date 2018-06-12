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

mutex_k threading_create_mutex (void)
{
    struct mutex * minst = (struct mutex *) kmalloc(sizeof(struct mutex), GFP_KERNEL);
    if (!minst)
        panic("Xenus kernel requested mutex, Linux said no.");
    mutex_init(minst);
    return (mutex_k) minst;
}

void threading_mutex_destroy(mutex_k mutex)
{
    mutex_destroy(mutex);
    kfree(mutex);
}

void init_mutex(bootstrap_t * functions)
{
    functions->mutex.mutex_init         = threading_create_mutex;
    functions->mutex.mutex_lock         = mutex_lock;
    functions->mutex.mutex_unlock       = mutex_unlock;
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
task_k threading_create_thread_unsafe(thread_callback_t callback, void * data, char * name)
{
    char * buffer;
    
    if (!callback)
        return NULL;
    
    buffer = (char *) kmalloc(sizeof(size_t) * 2, GFP_KERNEL);
    
    *(size_t *)(buffer + 0)              = (size_t) callback;
    *(size_t *)(buffer + sizeof(size_t)) = (size_t) data;
    
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
    preempt_enable();
}

void threading_preempt_disable(void)
{
    preempt_enable();
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

void init_cpu(bootstrap_t * functions)
{
    functions->cpu.kernel_fpu_begin         = threading_kernel_fpu_begin;
    functions->cpu.kernel_fpu_end           = threading_kernel_fpu_end;
    functions->cpu.create_thread_unsafe     = threading_create_thread_unsafe;
    functions->cpu.get_current_pid          = threading_get_pid;
    functions->cpu._current                 = threading__current;
    functions->cpu.tls                      = threading_tls;
    functions->cpu.preempt_enable           = threading_preempt_enable;
    functions->cpu.preempt_disable          = threading_preempt_disable;
}

void *  mem_execalloc(size_t size)
{
    return __vmalloc(size, GFP_KERNEL, PAGE_KERNEL_EXEC);
}

void init_memory(bootstrap_t * functions)
{       
    functions->memory.execalloc     = mem_execalloc;  
    functions->memory.execfree      = vfree;   
    functions->memory.kmalloc       = kmalloc;   
    functions->memory.vmalloc       = vmalloc; 
    functions->memory.kfree         = kfree;     
    functions->memory.vfree         = vfree;    
    functions->memory.ksize         = ksize;    
    functions->memory.memcpy        = memcpy;      
    functions->memory.memset        = memset;     
    functions->memory.memmove       = memmove;    
    functions->memory.memcmp        = memcmp;     
    functions->memory.memscan       = memscan;   
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
    printk("%s\n", msg); //puts isn't a symbol?
    while (1)
        ssleep(2000);
}

void test_function(int a_1, int a_2, int a_3, int a_4, int a_5, int a_6, int a_7, int a_8, int a_9, int a_10, int a_11, int a_12)
{
    printk("Microsoft to SystemV test (%i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i) \n",
           a_1, a_2, a_3, a_4, a_5, a_6, a_7, a_8, a_9, a_10, a_11, a_12);
}

void init_dbg(bootstrap_t * functions)
{
    functions->dbg.test_function = test_function;
    functions->dbg.panic         = dbg_panic; //= panic;
    functions->dbg.print         = dbg_print;
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