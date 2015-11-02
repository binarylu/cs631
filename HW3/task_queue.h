#ifndef __TASK_QUEUE_H__
#define __TASK_QUEUE_H__

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "type.h"

typedef union {
    _task *task;
    _response *response;
} _ele;

typedef struct __task_queue_entry {
    _ele element;
    struct __task_queue_entry *next;
} _task_queue_entry;

typedef enum {TASK, RESPONSE} enum_q_type;

extern void q_init(enum_q_type t);
extern _ele q_get(enum_q_type t);
extern int q_add(enum_q_type t, _ele element);
extern int q_isEmpty(enum_q_type t);
extern void q_destroy(enum_q_type t);
extern void q_print(enum_q_type t);

#endif /* end of include guard: __TASK_QUEUE_H__ */
