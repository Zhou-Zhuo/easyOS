#define SCHEDULER_C

#include <common.h>
#include <scheduler.h>
#include <cpu.h>


void systick_isr()
{
	task_t *task;
	uint32_t cpu_sr;

	__ENTER_CRITICAL();
	
	for(task = task_list; task != NULL; task = task->next) {
		if(task->timeout) {
			if(--(task->timeout) == 0 /*&& task->state != TASK_PEND*/) {
				if(task->state == TASK_PEND) 
					;//wake up 
				make_ready(task);
			}
		}
	}

	__EXIT_CRITICAL();

	schedule();

}

void schedule()
{
	uint32_t cpu_sr;

	__ENTER_CRITICAL();
	find_high_ready_task();

	if(high_ready_task != current_task) {
		__EXIT_CRITICAL();  //warning: think twice about this
		context_switch();   //will not return
	}
	else {
		__EXIT_CRITICAL();
		return;
	}
	
}


inline void make_ready(task_t *task)
{
	prio_t priority;
	uint32_t cpu_sr;

	__ENTER_CRITICAL();

	priority = task->priority;
	task_ready_grp |= 1 << (priority>>3);
	task_ready_bmp[priority>>3] |= 1 << (priority&0x07);

	__EXIT_CRITICAL();

}

inline void make_unready(task_t *task)
{
	prio_t priority;
	uint32_t cpu_sr;

	__ENTER_CRITICAL();

	priority = task->priority;
	task_ready_bmp[priority>>3] &= ~(1 << (priority&0x07));
	if(!task_ready_bmp[priority>>3])
	task_ready_grp &= ~(1 << (priority>>3));

	__EXIT_CRITICAL();
}

inline void find_high_ready_task()
{
	prio_t high_ready_prio;
	uint32_t cpu_sr;

	__ENTER_CRITICAL();

	high_ready_prio = unmap_tbl[ task_ready_grp ];
	high_ready_prio = unmap_tbl[ task_ready_bmp[ high_ready_prio ] ] + high_ready_prio*8;
	high_ready_task = prio_tbl[ high_ready_prio ];
	
	__EXIT_CRITICAL();

}

void sleep_ticks(uint32_t ticks)
{
	uint32_t cpu_sr;

	__ENTER_CRITICAL();

	current_task->timeout = ticks;
	make_unready(current_task);

	__EXIT_CRITICAL();

	schedule();

}
