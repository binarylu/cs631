#ifndef __NET_H__
#define __NET_H__

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>

#include <string.h>

#include "util.h"

#define MAX_LISTEN 10

typedef struct {
    char *address;
    uint16_t port;
} _net_arg;

extern int init_socket(_net_arg *arg);

#endif /* end of include guard: __NET_H__ */
