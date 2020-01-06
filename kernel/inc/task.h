#ifndef TASK_H
#define TASK_H

#ifdef TASK_C
#define __TASK_EXTERN 
#else 
#define __TASK_EXTERN extern 
#endif

#include <common.h>
#include <type.h>
#include <config.h>
#include <bitmap.h>
#include <event.h>

typedef struct Task {
	/* stack top 
	 * 栈顶放在最上面方便上下文切换，跟uC/OSII学的 */ 
	stack_t *sp;

	/* stack bottom */
	stack_t *stack_bottom;
	//*it don't have a stack cheak, so a field for stack size is useless

	/* entry of code */
	void (*entry) (void);

	/* task id */
	tid_t  tid;

	/* init priority 
	 * TO DO: 增加临时优先级以应付优先级反转 */
	prio_t  priority;

	/* sleep ticks */
	uint32_t timeout;

	/* waiting event */
	event_obj *event_wait;

	/* task state */
	task_state_t  state;

	struct Task  *prev; //Any need to use a double linked list?删除的时候需要啊!
	struct Task  *next;	

} task_t;

/* task states */
#define TASK_READY 1
#define TASK_PEND  2
#define TASK_NOT_USED 3

#define LOWIST_PRIO (MAX_PRIO_NR - 1)


/* task_t pool, indexed by tid */
__TASK_EXTERN task_t task_struct_pool[MAX_TASK_NR];

__TASK_EXTERN task_t *current_task;

__TASK_EXTERN volatile task_t *high_ready_task;

/* bitmap that record the ready prioty */
//__TASK_EXTERN uint8_t task_ready_grp;
//__TASK_EXTERN uint8_t task_ready_bmp[MAX_PRIO_NR / 8];
__TASK_EXTERN bitmap_t task_ready_bitmap;

///* bitmap that record the tid used */ //bad idea
//uint8_t task_id_grp;
//uint8_t task_id_bmp[MAX_TASK_NR/8];

__TASK_EXTERN task_t *task_list;

__TASK_EXTERN task_t *free_task_list;

/* 以tid索引task_t的表 */
//TASK_EXTERN task_t *tid_index_tbl[MAX_TASK_NR]; //look at line 4 

/* 以priority索引task struct的表 */
__TASK_EXTERN task_t *prio_tbl[MAX_PRIO_NR];

/* port out */
void task_struct_init();

err_t task_create(tid_t *tid, void (*entry) (void), prio_t priority,
		  stack_t * stack_bottom);

err_t task_delete(tid_t tid);

tid_t get_current_id();

	
#endif
