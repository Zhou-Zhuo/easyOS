#define MAILBOX_C

#include <common.h>
#include <error.h>
#include <cpu.h>
#include <task.h>
#include <scheduler.h>
#include <event.h>
#include <mailbox.h>

static inline void mailbox_timeout_handler(task_t *ptask, event_obj *pevent_o);

err_t mailbox_create(eid_t *eid)
{
	cpu_sr_t cpu_sr;
	event_obj *pevent_o = NULL;
	mailbox_t *pmailbox = NULL;

	__ENTER_CRITICAL();

	pevent_o = (event_obj *)get_event_struct();

	if(!pevent_o) {
		__EXIT_CRITICAL();
		return ERR_NO_MORE_ECB;//NO_MORE_EVENT_STRUCT
	}

	pmailbox = (mailbox_t *)pevent_o;

	pevent_o->type = EV_MAILBOX;
	pevent_o->timeout_handle = mailbox_timeout_handler; //timeout handler
	bmp_reset(&(pevent_o->wait_task_bitmap)); 
	*eid = pevent_o->eid;

	pmailbox->data = NULL;
	
	__EXIT_CRITICAL();

	return ERR_NONE;
}

err_t mailbox_delete(eid_t eid)
{
	cpu_sr_t cpu_sr;
	event_obj *pevent_o = NULL;
	mailbox_t *pmailbox = NULL;

	__ENTER_CRITICAL();

	if(eid > MAX_EVENT_NR -1) {
		__EXIT_CRITICAL();
		return ERR_NO_SUCH_EVENT;
	}

	pevent_o = (event_obj *)&event_struct_pool[eid];

	if(pevent_o->type != EV_MAILBOX) {
		__EXIT_CRITICAL();
		return ERR_EV_TYPE;  //NOT A MAILBOX
	}
	if(pevent_o->wait_task_bitmap.group != 0x00) {
		__EXIT_CRITICAL();
		return ERR_EV_BUSY;  //EVENT_BUSY
	}

	pmailbox = (mailbox_t *)pevent_o;

	pevent_o->type = EV_NULL;

	pmailbox->data = NULL;

	return_event_struct((event_un *)pevent_o);

	__EXIT_CRITICAL();

	return ERR_NONE;
}

err_t mailbox_pend(eid_t eid, void **pdata, uint32_t timeout)
{
	cpu_sr_t cpu_sr;
	event_obj *pevent_o = NULL;
	mailbox_t *pmailbox = NULL;

	__ENTER_CRITICAL();

	if(eid > MAX_EVENT_NR -1 ) {
		__EXIT_CRITICAL();
		return ERR_NO_SUCH_EVENT;
	}

	pevent_o = (event_obj *)&event_struct_pool[eid];
	 
	if(pevent_o->type != EV_MAILBOX) {
		__EXIT_CRITICAL();
		return ERR_EV_TYPE;    //NOT A MAILBOX
	}

	pmailbox = (mailbox_t *)pevent_o;

	/* set bit according to priority */
	bmp_setbit_ne(&(pevent_o->wait_task_bitmap), current_task->priority);

	current_task->state = TASK_PEND;
	current_task->event_wait = pevent_o;
	current_task->timeout = timeout;  
	make_unready(current_task);
	__EXIT_CRITICAL();

	schedule();    //will return

	__ENTER_CRITICAL();

	if(timeout != 0 && current_task->timeout == 0) {
		__EXIT_CRITICAL();
		return ERR_EV_TIMEOUT;  //EVENT_TIMEOUT
	}

	current_task->timeout = 0;
	*pdata = pmailbox->data;

	__EXIT_CRITICAL();

	return 0;

}

err_t mailbox_post(eid_t eid, void *data)
{
	cpu_sr_t cpu_sr;
	event_obj *pevent_o = NULL;
	mailbox_t *pmailbox = NULL;
	task_t *ptask = NULL;
	prio_t priority;
	bitmap_t *pwaitbmp = NULL;

	__ENTER_CRITICAL();

	if(eid > MAX_EVENT_NR -1) {
		__EXIT_CRITICAL();
		return ERR_NO_SUCH_EVENT;
	}

	pevent_o = (event_obj *)&event_struct_pool[eid];

	if(pevent_o->type != EV_MAILBOX) {
		__EXIT_CRITICAL();
		return ERR_EV_TYPE;
	}

	pmailbox = (mailbox_t *)pevent_o;

	pmailbox->data = data;

	pwaitbmp = &(pevent_o->wait_task_bitmap);
	while(pwaitbmp->group) {     //will global data optimized? it may change inside the function call
		priority = bmp_getmin_s(pwaitbmp);
		ptask = prio_tbl[priority];
		ptask->state = TASK_READY;
		ptask->event_wait = NULL;
		bmp_clearbit_ne(pwaitbmp, priority);
		make_ready(ptask);
	}

	__EXIT_CRITICAL();

	schedule();

	return ERR_NONE;

}

static inline void mailbox_timeout_handler(task_t *ptask, event_obj *pevent_o)
{
	bmp_clearbit_ne(&(pevent_o->wait_task_bitmap), ptask->priority);
}

