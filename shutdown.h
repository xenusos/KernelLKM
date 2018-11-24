typedef void(XENUS_MS_ABI * shutdown_handler_p)(void);

void shutdown_set_handler(shutdown_handler_p);
void shutdown_trigger(void);