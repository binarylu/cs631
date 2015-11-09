#include "net.h"

static int init_net(char *address, char *port, int *sock);

int
init_net(char *address, char *port, int *sock)
{
    int s, v, i;
    struct addrinfo hints, *result, *rp;
    struct sockaddr_in server_addr;

    memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE | AI_NUMERICHOST;
	hints.ai_protocol = IPPROTO_IP;
	/*hints.ai_canonname = NULL;
	hints.ai_addr = (struct sockaddr *)&server;
	hints.ai_next = NULL;*/

    if ((s = getaddrinfo(address, port, &hints, &result)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    for (rp = result, i = 0; rp != NULL && i < 2; rp = rp->ai_next, ++i) {
        sock[i] = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sock[i] == -1) {
            perror("socket");
            continue;
        }
        v = 1;
        if (setsockopt(sock[i], SOL_SOCKET, SO_REUSEADDR, &v, sizeof(v)) < 0) {
            perror("setsockopt");
            return -1;
        }
        if (bind(sock[i], rp->ai_addr, rp->ai_addrlen) < 0) {
            perror("socket");
            return -1;
        }
        if (listen(sock[i], MAX_LISTEN) < 0) {
            perror("listen");
            return -1;
        }
    }
    freeaddrinfo(result);

    return 0;
}

void
network_loop(char *address, char *port)
{
    int listen_sock[2], connfd, fd_max = -1;
    struct sockaddr_in client_addr;
    socklen_t client_addrlen;
    fd_set fdset, rset;
    int nread;
    char buf[BUFFSIZE];
    _client_info client[FD_SETSIZE];
    int i, m, maxi = -1;

    client_addrlen = sizeof(struct sockaddr_in);

    if (init_net(address, port, listen_sock) < 0) {
        fprintf(stderr, "Fail to initial network!\n");
        return;
    }

    for (i = 0; i < FD_SETSIZE; ++i)
        client[i].fd = -1;

    fd_max = listen_sock[0] > listen_sock[1] ? listen_sock[0] : listen_sock[1];
    FD_ZERO(&fdset);
    FD_SET(listen_sock[0], &fdset);
    FD_SET(listen_sock[1], &fdset);
    for (;;) {
        rset = fdset;
        if (select(fd_max + 1, &rset, NULL, NULL, NULL) < 0) {
            perror("select");
            exit(EXIT_FAILURE);
        }
        for (m = 0; m < 2; ++m) {
            if (FD_ISSET(listen_sock[m], &rset)) {
                if ((connfd = accept(listen_sock[m], (struct sockaddr *)&client_addr, &client_addrlen)) <= 0) {
                    perror("accept");
                } else {
                    for (i = 0; i < FD_SETSIZE; ++i) {
                        if (client[i].fd < 0) {
                            client[i].fd = connfd;
                            client[i].addr = client_addr;
                            printf("Get connection from %s.\n", inet_ntoa(client[i].addr.sin_addr));
                            break;
                        }
                    }
                    if (i == FD_SETSIZE)
                        fprintf(stderr, "Too many connections.");
                    FD_SET(connfd, &fdset);
                    if (connfd > fd_max)
                        fd_max = connfd;
                    if (i > maxi)
                        maxi = i;
                }
            }
        }
        for (i = 0; i <= maxi; ++i) {
            if ((connfd = client[i].fd) < 0)
                continue;
            if (FD_ISSET(connfd, &rset)) {
                nread = read(connfd, buf, BUFFSIZE);
                if (nread < 0) {
                    perror("read");
                    close(connfd);
                } else if (nread == 0) {
                    fprintf(stdout, "Connection with client %s is closed\n", inet_ntoa(client_addr.sin_addr));
                    FD_CLR(connfd, &fdset);
                    close(connfd);
                    client[i].fd = -1;
                } else {
                    if (strncmp(buf, "CLOSE", 5) == 0) {
                        fprintf(stdout, "Client %s closes the connection\n", inet_ntoa(client_addr.sin_addr));
                        FD_CLR(connfd, &fdset);
                        close(connfd);
                        client[i].fd = -1;
                        break;
                    }
                    buf[nread] = 0;
                    fprintf(stdout, "Client %s says: %s\n", inet_ntoa(client_addr.sin_addr), buf);
                }
            }
        }
    }
    close(listen_sock[0]);
    close(listen_sock[1]);
}
