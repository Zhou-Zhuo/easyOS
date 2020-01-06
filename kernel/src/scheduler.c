#define SCHEDULER_C

#include <common.h>
#include <bitmap.h>
#include <scheduler.h>
#include <cpu.h>


void systick_isr()
{
	task_t *ptask = NULL;
	cpu_sr_t cpu_sr;

	__ENTER_CRITICAL();
	
	for(ptask = task_list; ptask != NULL; ptask = ptask->next) {
		if(ptask->timeout) {
			if(--(ptask->timeout) == 0 /*&& ptask->state != TASK_PEND*/) {
				if(ptask->state == TASK_PEND) {
					ptask->event_wait->timeout_handle(ptask,
						   ptask->event_wait);
					ptask->state = TASK_READY;
				}  //wake up, and clear event wait bit 
				make_ready(ptask);
			}
		}
	}

	__EXIT_CRITICAL();

	schedule();

}

void schedule()
{
	cpu_sr_t cpu_sr;

	__ENTER_CRITICAL();
	find_high_ready_task();

	if(high_ready_task != current_task) {
		__EXIT_CRITICAL();  //warning: think twice about this
		context_switch();   //WILL MUST RETURN!
	}
	else {
		__EXIT_CRITICAL();
		return;
	}
	
}


inline void make_ready(task_t *task)
{
	cpu_sr_t cpu_sr;

	__ENTER_CRITICAL();

	bmp_setbit_ne(&task_ready_bitmap, task->priority);

	__EXIT_CRITICAL();

}

inline void make_unready(task_t *task)
{
	cpu_sr_t cpu_sr;

	__ENTER_CRITICAL();

	bmp_clearbit_ne(&task_ready_bitmap, task->priority);

	__EXIT_CRITICAL();
}

inline void find_high_ready_task()
{
	cpu_sr_t cpu_sr;

	__ENTER_CRITICAL();

	high_ready_task = prio_tbl[ bmp_getmin_s(&task_ready_bitmap) ];

	__EXIT_CRITICAL();

}

void sleep_ticks(uint32_t ticks)
{
	cpu_sr_t cpu_sr;

	__ENTER_CRITICAL();

	current_task->timeout = ticks;
	make_unready(current_task);

	__EXIT_CRITICAL();

	schedule();

}

