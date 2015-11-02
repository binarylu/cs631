#ifndef __HANDLE_H_
#define __HANDLE_H_

#include <sys/socket.h>
#include <errno.h>

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "type.h"

#define BUFFSIZE 256

int handle_task(_task *task, _response *response);

#endif /* end of include guard: __HANDLE_H_ */
