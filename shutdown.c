#include "common.h"
#include "shutdown.h"

static shutdown_handler_p shutdown_callback;

void shutdown_set_handler(shutdown_handler_p callback)
{
	shutdown_callback = callback;
}

void shutdown_trigger(void)
{
	shutdown_callback();
}