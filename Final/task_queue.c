#include "task_queue.h"

static pthread_mutex_t task_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t response_mutex = PTHREAD_MUTEX_INITIALIZER;
static struct __task_queue_entry *task_head, *task_tail;
static struct __task_queue_entry *response_head, *response_tail;

void
q_init(enum_q_type t) {
	if (t == TASK) {
		task_head = NULL;
		task_tail = NULL;
	} else if (t == RESPONSE) {
		response_head = NULL;
		response_tail = NULL;
	}
}

_ele
q_get(enum_q_type t) {
	_ele element = {NULL};
	pthread_mutex_t *mutex = NULL;
	_task_queue_entry **head = NULL;
	if (t == TASK) {
		mutex = &task_mutex;
		head = &task_head;
	} else if (t == RESPONSE) {
		mutex = &response_mutex;
		head = &response_head;
	} else {
		return element;
	}

    pthread_mutex_lock(mutex);
    if (*head == NULL) {
        return element;
    }
    element = (*head)->element;
    (*head) = (*head)->next;
    pthread_mutex_unlock(mutex);
    return element;
}

int
q_add(enum_q_type t, _ele element) {
    _task_queue_entry *node;
	pthread_mutex_t *mutex = NULL;
	_task_queue_entry **head = NULL;
	_task_queue_entry **tail = NULL;
	if (t == TASK) {
		mutex = &task_mutex;
		head = &task_head;
		tail = &task_tail;
	} else if (t == RESPONSE) {
		mutex = &response_mutex;
		head = &response_head;
		tail = &response_tail;
	} else {
		return -1;
	}

    node = (_task_queue_entry *)malloc(sizeof(_task_queue_entry));
    if (node == NULL) {
        return -1;
    }
    node->element = element;
    node->next = NULL;
    pthread_mutex_lock(mutex);
    if (*head == NULL) {
        *head = node;
    } else {
        (*tail)->next = node;
    }
    *tail = node;
    pthread_mutex_unlock(mutex);
    return 0;
}

int
q_isEmpty(enum_q_type t) {
	int ret = 0;
	pthread_mutex_t *mutex = NULL;
	_task_queue_entry **head = NULL;
	if (t == TASK) {
		mutex = &task_mutex;
		head = &task_head;
	} else if (t == RESPONSE) {
		mutex = &response_mutex;
		head = &response_head;
	} else {
		return 1;
	}
    pthread_mutex_lock(mutex);
	ret = *head == NULL ? 1 : 0;
    pthread_mutex_unlock(mutex);
	return ret;
}

void
q_destroy(enum_q_type t) {
	pthread_mutex_t *mutex = NULL;
	if (t == TASK) {
		mutex = &task_mutex;
	} else if (t == RESPONSE) {
		mutex = &response_mutex;
	}
	pthread_mutex_destroy(mutex);
}

void
q_print(enum_q_type t) {
    _task_queue_entry *p;
	pthread_mutex_t *mutex = NULL;
	_task_queue_entry *head = NULL;
	if (t == TASK) {
		mutex = &task_mutex;
		head = task_head;
	} else if (t == RESPONSE) {
		mutex = &response_mutex;
		head = response_head;
	} else {
		return;
	}

    pthread_mutex_lock(mutex);
    p = head;
    while (p != NULL) {
		if (t == TASK) {
			printf("%d -> ", (p->element).task->fd);
		} else if (t == RESPONSE) {
			printf("%d -> ", p->element.response->fd);
		}
        p = p->next;
    }
    printf("NULL\n");
    pthread_mutex_unlock(mutex);
}
