#include "net.h"

int
init_net(char *address, char *port)
{
    int s;
    struct addrinfo hints;
    struct addrinfo *result, *rp;

    struct sockaddr_in server;
    struct sockaddr_in6 server6;

    char ip[INET6_ADDRSTRLEN];
    memset(ip, 0, INET6_ADDRSTRLEN);

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE | AI_NUMERICHOST;
	hints.ai_protocol = IPPROTO_IP;
	hints.ai_canonname = NULL;
	hints.ai_addr = (struct sockaddr *)&server;
	hints.ai_next = NULL;

    if ((s = getaddrinfo(address, port, &hints, &result)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        if (rp->ai_family == AF_INET) {
            printf("ip_v4\n");
        }
        else if (rp->ai_family == AF_INET6)
            printf("ip_v6\n");
        else
            printf("unknow\n");
    }
    sockaddr2string(, NULL);
}

int
sockaddr2string(struct sockaddr *sa, char *address)
{
    char ip[INET6_ADDRSTRLEN] = {0};
    struct sockaddr_in *server;
    struct sockaddr_in6 *server6;
    if (sa->sa_family == AF_INET) {
        server = (struct sockaddr_in *)sa;
        if (inet_ntop(sa->sa_family, &(server->sin_addr), ip, INET_ADDRSTRLEN) == NULL) {
            perror("inet_ntop");
        }
        printf("%s\n", ip);
    } else {
        server6 = (struct sockaddr_in6 *)sa;
        if (inet_ntop(sa->sa_family, &(server6->sin6_addr), ip, INET_ADDRSTRLEN) == NULL) {
            perror("inet_ntop");
        }
        printf("%s\n", ip);
    }
    return 0;
}
