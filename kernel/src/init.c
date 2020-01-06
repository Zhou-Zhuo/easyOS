#include <task.h>
#include <event.h>
#include <scheduler.h>
#include <cpu.h>
#include <serial.h>
#include <common.h>

void misc_init();

void sys_init()
{
	task_struct_init();
	event_struct_init();
	misc_init();
}

void misc_init()
{
	os_ready = False;
	interrupt_nesting = 0;
}

void startup()
{

	printf("OS start!\r\n");

	find_high_ready_task();
	first_switch();

	while(1); //will never reach here
}


