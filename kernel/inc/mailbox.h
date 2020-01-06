#ifndef MAILBOX_H
#define MAILBOX_H

#include <type.h>
//#include <event.h>
struct event_obj;

typedef struct mailbox_t {
	/* inherit from event_obj */
	struct event_obj parent;
	/* data transferred */
	void *data;
} mailbox_t;

err_t mailbox_create(eid_t *eid);

err_t mailbox_delete(eid_t eid);

err_t mailbox_pend(eid_t eid, void **pdata, uint32_t timeout);

err_t mailbox_post(eid_t eid, void *data);

#endif

