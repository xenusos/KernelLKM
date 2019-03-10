/*
    Purpose: 
    Author: Reece W.
    License: All Rights Reserved J. Reece Wilson
*/
#include "common.h"
#include "shutdown.h"

static shutdown_handler_p shutdown_callback = NULL;

void shutdown_set_handler(shutdown_handler_p callback)
{
    shutdown_callback = callback;
}

void shutdown_trigger(void)
{
    if (shutdown_callback)
        shutdown_callback();
}