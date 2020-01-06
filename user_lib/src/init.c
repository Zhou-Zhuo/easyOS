#include "init.h" 

extern serial_t com1;
extern serial_t *console_out, *console_in;

void serial_init()
{
	com1_init();
}

void console_init()
{
	console_out = &com1;
	console_in = &com1;
}


