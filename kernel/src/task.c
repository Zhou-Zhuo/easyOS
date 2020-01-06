#define TASK_C

#include <common.h>		//this will be move to common.h
#include <error.h>
#include <task.h>
#include <scheduler.h>
#include <cpu.h>


void task_struct_init()
{
    uint8_t i;

    /* assign an id to every task structure, start with 0 */
    for (i = 0; i < MAX_TASK_NR; i++) {
		task_struct_pool[i].tid = i;
    }

    /* initialize free task structure lisk */
    free_task_list = task_struct_pool;
    task_list = NULL;

    for (i = 0; i < MAX_TASK_NR - 1; i++) {
		task_struct_pool[i].prev = NULL;   //空闲链表从一个方向存取,只用next链接了,省事
		task_struct_pool[i].next = &task_struct_pool[i + 1];
    }
	task_struct_pool[MAX_TASK_NR - 1].prev = NULL;
    task_struct_pool[MAX_TASK_NR - 1].next = NULL;

}

inline task_t *get_task_struct()
{
	cpu_sr_t cpu_sr;
    task_t *ptask = NULL;

	__ENTER_CRITICAL();

    ptask = free_task_list;
    free_task_list = free_task_list->next;

	if(task_list)
		task_list->prev = ptask;
    ptask->next = task_list;
	ptask->prev = NULL;
    task_list = ptask;

	__EXIT_CRITICAL();

    return ptask;
}

inline void return_task_struct(task_t *ptask)
{
	cpu_sr_t cpu_sr;

	__ENTER_CRITICAL();

	if(ptask->next)
		ptask->next->prev = ptask->prev;
	if(ptask->prev)
		ptask->prev->next = ptask->next;
	else 
		task_list = ptask->next;
	ptask->next = free_task_list;
	ptask->prev = NULL;
	free_task_list = ptask;

	__EXIT_CRITICAL();
}

void task_stack_init(task_t * ptask)
{
    stack_t **sp = &ptask->sp;
    /* R1-R4,R12,PC,LR,xPSR */
    *((*sp)--) = 0X01000000;	//xPSR
    *((*sp)--) = (uint32_t) (ptask->entry);	//PC
    *((*sp)--) = 0X14141414;	//LR
    *((*sp)--) = 0X12121212;	//R12
    *((*sp)--) = 0X03030303;	//R3
    *((*sp)--) = 0X02020202;	//R2
    *((*sp)--) = 0X01010101;	//R1
    *((*sp)--) = 0X00000000;	//R0
    /* R4-R11 */
    *((*sp)--) = 0X11111111;
    *((*sp)--) = 0X10101010;
    *((*sp)--) = 0X09090909;
    *((*sp)--) = 0X08080808;
    *((*sp)--) = 0X07070707;
    *((*sp)--) = 0X06060606;
    *((*sp)--) = 0X05050505;
    *((*sp)) = 0X04040404;

}

err_t task_create(tid_t *tid, void (*entry) (void), prio_t priority,
		  stack_t * stack_bottom)
{
	cpu_sr_t cpu_sr;
    task_t *ptask = NULL;

    __ENTER_CRITICAL();
	
	if(priority >= LOWIST_PRIO)
		if(prio_tbl[LOWIST_PRIO]) {
			__EXIT_CRITICAL();
			return ERR_NO_SUCH_PRIO;  //cannot take the same priority with system task
		}

    /* get new task struct */
    ptask = get_task_struct();
	if(ptask == NULL) {
		__EXIT_CRITICAL();
		return ERR_NO_MORE_TCB;  //NO_MORE_TASK_STRUCT
	}
    ptask->entry = entry;
    ptask->priority = priority;
    ptask->stack_bottom = stack_bottom;
	ptask->state = TASK_READY;
	ptask->event_wait = NULL;
	ptask->timeout = 0;
    ptask->sp = stack_bottom;
    task_stack_init(ptask);

	*tid = ptask->tid;

	prio_tbl[priority] = ptask;

	/* get ready to run */
	make_ready(ptask);
	if(os_ready) {
		__EXIT_CRITICAL();
		schedule();
	}

	__EXIT_CRITICAL();

	return ERR_NONE;
}

err_t task_delete(tid_t tid)
{
	cpu_sr_t cpu_sr;
	task_t *ptask = NULL;

	__ENTER_CRITICAL();

	if(tid > MAX_TASK_NR) {
		__EXIT_CRITICAL();
		return ERR_NO_SUCH_TASK;
	}

	ptask = &task_struct_pool[tid];

	if(ptask->state == TASK_NOT_USED) {  //How about pending?
		__EXIT_CRITICAL();
		return ERR_NO_SUCH_TASK;     //REMMEMBER TO RELEASE EVENT !!
	}

	if(ptask->priority == LOWIST_PRIO) {
		__EXIT_CRITICAL();
		return ERR_SYS_TASK; //cannot delete system task
	}

	if(ptask->state == TASK_PEND) 
		ptask->event_wait->timeout_handle(ptask, ptask->event_wait);

	ptask->state = TASK_NOT_USED;

	prio_tbl[ptask->priority] = NULL;
	make_unready(ptask);
	return_task_struct(ptask);

	__EXIT_CRITICAL();
	schedule();

	return ERR_NONE;
}

tid_t get_current_id()
{
	return current_task->tid;
}

