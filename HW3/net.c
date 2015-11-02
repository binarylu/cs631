#include "net.h"

int
init_socket(_net_arg *arg) {
    int sock;
    int v;
    struct sockaddr_in server_addr;
    socklen_t length;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        /*TODO: debug mode and error handle*/
        perror("socket");
        return -1;
    }
    if (setnonblocking(sock) < 0) {
        /*TODO: debug mode and error handle*/
        perror("setnonblocking");
        return -1;
    }
    v = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &v, sizeof(v)) < 0) {
        /*TODO: debug mode and error handle*/
        perror("setsockopt");
        return -1;
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(arg->address);
    server_addr.sin_port = htons(arg->port);
    memset(&server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));
    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0) {
        /*TODO: debug mode and error handle*/
        perror("bind");
        return -1;
    }

    length = sizeof(server_addr);
    if (getsockname(sock, (struct sockaddr *)&server_addr, &length)) {
        perror("getting socket name");
        return -1;
    }
    arg->port = ntohs(server_addr.sin_port);

    if (listen(sock, MAX_LISTEN) < 0) {
        /*TODO: debug mode and error handle*/
        perror("listen");
        return -1;
    }
    return sock;
}
