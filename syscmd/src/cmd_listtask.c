#include <user_lib.h>
#include <cpu.h>
#include <task.h>
#include <event.h>

int do_lstsk(int argc, char *argv[])
{
	task_t *ptask = NULL;

	if(argc != 0)
		goto USAGE_ERR;

	for(ptask = task_list; ptask ; ptask = ptask->next) {
		printf("Task %d, priority: %d, state: %s ", ptask->tid, ptask->priority, ptask->state == 1 ? "ready":"pending");
		if(ptask->state == 1)
			printf("\r\n");
		else
			printf("on event %d\r\n", ptask->event_wait->eid);
	}

	return 1;

USAGE_ERR:
	print_usage();
	return 0;
}

INCOM_SH_CMD(lstsk, do_lstsk, 
		"lstsk - list tasks in the system.\r\n");

