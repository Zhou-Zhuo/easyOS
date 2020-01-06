#define EVENT_C

#include <event.h>
#include <common.h>
#include <cpu.h>

void event_struct_init()
{
	uint8_t i = 0;
	event_obj *pevent_o = NULL;
	
	for(i = 0; i < MAX_EVENT_NR; i ++) {
		pevent_o = (event_obj *)&event_struct_pool[i];
		pevent_o->eid = i;
	}
}

inline event_un *get_event_struct()
{
	cpu_sr_t cpu_sr;
	eid_t eid;
	event_un *event = NULL;

	__ENTER_CRITICAL();

	eid = bmp_getmin_r(&event_used_bitmap);
	if(eid == 0 ) //eid == 0 might means event struct has been used out
		if(event_used_bitmap.group == 0xff) {
			__EXIT_CRITICAL();
			return NULL;
		}
	bmp_setbit_f(&event_used_bitmap, eid);
	event = &event_struct_pool[eid];

	__EXIT_CRITICAL();

	return event;
}

inline void return_event_struct(event_un *event)
{
	cpu_sr_t cpu_sr;

	__ENTER_CRITICAL();
	bmp_clearbit_f(&event_used_bitmap, ((event_obj *)event)->eid);
	__EXIT_CRITICAL();
}


