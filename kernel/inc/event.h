#ifndef EVENT_H
#define EVENT_H

#include <config.h>
#include <type.h>
#include <bitmap.h>

#ifdef EVENT_C
#define __EVENT_EXTERN 
#else 
#define __EVENT_EXTERN extern
#endif

typedef enum {
	EV_NULL,  //unused
	EV_MAILBOX,
	EV_SEM
} event_type_t;

typedef struct event_obj {
	/* event id */
	eid_t   eid;
	/* event type */
	event_type_t  type;
	/* bitmap that record task waitting on this event */
	bitmap_t   wait_task_bitmap;
	/* timeout handler */
	void  (*timeout_handle)();
} event_obj;

#include <mailbox.h>
#include <event_union.h>  //太难看了，但是依赖关系太麻烦，以后想想办法

/* event union pool, indexed by eid */
__EVENT_EXTERN event_un event_struct_pool[MAX_EVENT_NR];

/* record event union been used */
__EVENT_EXTERN bitmap_t event_used_bitmap;


void event_struct_init();

inline event_un *get_event_struct();

inline void return_event_struct(event_un *event);

#endif

