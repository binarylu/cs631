#ifndef __TYPE_H__
#define __TYPE_H__

#include <arpa/inet.h>

typedef struct __task {
    int fd;
    struct sockaddr_in client_addr;
    char *request;
} _task;

typedef struct __response {
    int fd;
    char *response;
} _response;

#endif /* end of include guard: __TYPE_H__ */
