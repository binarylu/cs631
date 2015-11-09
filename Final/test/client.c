#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int server_fd;
    static char buf_send[4096], buf_recv[4096];
    int nsend, nrecv;

    struct sockaddr_in remote_addr;

    remote_addr.sin_family=AF_INET;
    remote_addr.sin_addr.s_addr = inet_addr(argv[1]);
    remote_addr.sin_port=htons(atoi(argv[2]));
    memset(&remote_addr.sin_zero, 0, sizeof(remote_addr.sin_zero));

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return -1;
    }

    if (connect(server_fd, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) < 0) {
        perror("connect");
        return -1;
    }

    snprintf(buf_send, 4096, "hello");
    nsend = send(server_fd, buf_send, strlen(buf_send), 0);
    if (nsend == -1) {
        perror("send");
    }
    /*nrecv = recv(server_fd, buf_recv, sizeof(buf_recv), 0);
    if (nrecv == -1) {
        perror("recv");
    }
    printf("recv: %s\n", buf_recv);

    if (nsend != nrecv) {
        printf("send: %s\n", buf_send);
        printf("recv: %s\n", buf_recv);
        return -1;
    }*/

    return 0;
}