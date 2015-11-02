#include "util.h"

int
setnonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        /*TODO: debug mode and error handle*/
        return -1;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        /*TODO: debug mode and error handle*/
        return -1;
    }
    return 0;
}
