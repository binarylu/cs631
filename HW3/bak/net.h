#ifndef __NET_H__
#define __NET_H__

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int init_net(char *address, char *port);
/*extern int sockaddr2string(struct sockaddr *sa, char *address);*/


#endif /* end of include guard: __NET_H__ */
