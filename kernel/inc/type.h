#ifndef TYPE_H
#define TYPE_H

#include <common.h>

typedef enum {
	False = 0,
	True = 1
} bool_t;

typedef int32_t err_t;
typedef uint8_t tid_t;
typedef uint8_t prio_t;
typedef uint8_t task_state_t;
typedef uint32_t stack_t;

typedef uint8_t eid_t;

#endif

