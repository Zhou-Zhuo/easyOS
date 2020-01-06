#ifndef EVENT_UNION_H
#define EVENT_UNION_H

#include <event.h>
//#include <mailbox.h>

struct mailbox_t;

typedef union {
	struct mailbox_t mailbox;
} event_un;

#endif

