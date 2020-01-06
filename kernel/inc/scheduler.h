#ifndef SCHEDULER_H
#define SCHEDULER_H

#ifdef SCHEDULER_C
#define __SCHEDULER_EXTERN    
#else 
#define __SCHEDULER_EXTERN extern
#endif 

#include <common.h>
#include <task.h>

__SCHEDULER_EXTERN bool_t os_ready;

__SCHEDULER_EXTERN uint32_t interrupt_nesting;

void systick_isr(void);
void schedule();
void make_ready(task_t *task);
void make_unready(task_t *task);
void find_high_ready_task();
void sleep_ticks(uint32_t ticks);


#endif

