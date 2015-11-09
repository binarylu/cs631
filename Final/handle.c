#include "handle.h"

int
handle_task(_task *task, _response *response) {
    int fd;
    int total, nrecv;
    char buf[BUFFSIZE] = {0};

    int data_size, n, nsend;

    fd = task->fd;
    total = 0;
    while ((nrecv = recv(fd, buf, sizeof(buf), 0)) > 0) {
        total += nrecv;
    }
    if (nrecv == 0) {
        printf("%s(%d) closed\n", inet_ntoa(task->client_addr.sin_addr),
            task->client_addr.sin_port);
        close(fd);
        return 0;
    }
    if (nrecv == -1 && errno != EAGAIN
            && errno != EWOULDBLOCK && errno != ECONNABORTED
            && errno != EPROTO && errno != EINTR) {
        perror("read error");
    }
    printf("%s(%d) says: ", inet_ntoa(task->client_addr.sin_addr),
            task->client_addr.sin_port);
    for (n = 0; n < strlen(buf); ++n) {
        printf("0x%x ", buf[n]);
    }
    printf("\n");

    data_size = strlen(buf);
    n = data_size;
    while (n > 0) {
        nsend = send(fd, buf + data_size - n, n, 0);
        if (nsend < n) {
            if (nsend == -1 && errno != EAGAIN) {
                perror("write error");
            }
            break;
        }
        n -= nsend;
    }

    return 0;
}
