#include <sys/epoll.h>
#include <pthread.h>
#include <errno.h>

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "net.h"
#include "task_queue.h"
#include "handle.h"

#define VERSION "0.1"
#define MAX_EVENTS 100
#define THREAD_NUM 20

pthread_mutex_t task_mutex;
pthread_cond_t task_available;

typedef struct __client_info {
    int fd;
    struct sockaddr_in client_addr;
} _client_info;


void usage(FILE *f);
void version();
int init_epoll(int sock);
int make_detached_thread(void *(*fn)(void *), void *arg);
void *thread_task(void *arg);
void loop(_net_arg *net_arg);
void handle_request(_client_info *client);

int main(int argc, char *argv[])
{
    int i, thread_cnt = 0;
    _net_arg net_arg = {"0.0.0.0", 0};
    /* getopt {{{*/
    int choice;
    while (1)
    {
        static struct option long_options[] = {
            {"version", no_argument,       0,    'v'},
            {"help",    no_argument,       0,    'H'},
            {"host",    required_argument, 0,    'h'},
            {"port",    required_argument, 0,    'P'},
            {0,0,0,0}
        };
        int option_index = 0;
        choice = getopt_long( argc, argv, "vh:P:", long_options, &option_index);
        if (choice == -1)
            break;
        switch( choice ) {
            case 'v':
                version();
                exit(EXIT_SUCCESS);
                break;
            case 'H':
                usage(stdout);
                exit(EXIT_SUCCESS);
                break;
            case 'h':
                net_arg.address = optarg;
                break;
            case 'P':
                net_arg.port = atoi(optarg);
                break;
            case '?':
                /*TODO add error handle*/
                break;
            default:
                return EXIT_FAILURE;
        }
    }

    /* Deal with non-option arguments here */
    /*if ( optind < argc )
      {
      while ( optind < argc )
      {

      }
      }*/
    /*}}}*/

    q_init(TASK);
    q_init(RESPONSE);
    if (pthread_mutex_init(&task_mutex, NULL) != 0) {
        /*TODO: debug mode and error handle*/
        return -1;
    }
    if (pthread_cond_init(&task_available, NULL) != 0) {
        /*TODO: debug mode and error handle*/
        return -1;
    }
    for (i = 0; i < THREAD_NUM; ++i) {
        if (make_detached_thread(thread_task, NULL) == 0) {
            ++thread_cnt;
        }
    }
#if LOG_LEVEL == 1
    /*TODO: use CBS LOG macro*/
    printf("Create %d/%d threads\n", thread_cnt, THREAD_NUM);
#endif
    loop(&net_arg);

    /*TODO: destroy all the resources and terminate threads*/
    q_destroy(TASK);
    q_destroy(RESPONSE);
    return 0;
}

void
usage(FILE *f) {
    fprintf(f, "Usage:\n");
}

void
version() {
    printf("Version: %s\n", VERSION);
}

int
init_epoll(int sock) {
    int epfd;
    struct epoll_event ev;
    /*
     * Since Linux 2.6.8, the size argument is unused, but must be greater than
     * zero.  (The kernel dynamically sizes the required data structures without
     * needing this initial hint.)
     */
    if ((epfd = epoll_create(MAX_EVENTS)) < 0) {
        /*TODO: debug mode and error handle*/
        perror("epoll_create");
        return -1;
    }

    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = sock;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &ev) < 0) {
        /*TODO: debug mode and error handle*/
        perror("epoll_ctl");
        return -1;
    }
    return epfd;
}

int
make_detached_thread(void *(*fn)(void *), void *arg) {
    int err;
    pthread_t tid;
    pthread_attr_t attr;

    err = pthread_attr_init(&attr);
    if (err != 0) {
        /*TODO: debug mode and error handle*/
        return err;
    }
    err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (err == 0) {
        err = pthread_create(&tid, &attr, fn, arg);
    }
    (void)pthread_attr_destroy(&attr);
    return err;
}

void *
thread_task(void *arg) {
    _ele element;
    _response *response;

    for (;;) {
        pthread_mutex_lock(&task_mutex);
        while (q_isEmpty(TASK)) {
            pthread_cond_wait(&task_available, &task_mutex);
        }
        element = q_get(TASK);
        pthread_cond_broadcast(&task_available);
        pthread_mutex_unlock(&task_mutex);

        response = (_response *)malloc(sizeof(_response));
        handle_task(element.task, response);
        free(element.task);
        element.response = response;
        while (q_add(RESPONSE, element) != 0) {
            printf("There is no enough for new response!\n");
            sleep(1);
        }
    }
    return NULL;
}

void loop(_net_arg *net_arg) {
    int i;
    int listen_sock, epfd;
    int nfds;
    struct sockaddr_in client_addr;
    socklen_t client_addrlen;
    struct epoll_event ev, events[MAX_EVENTS];
    int connfd;
    _client_info *client;

    client_addrlen = sizeof(struct sockaddr_in);
    listen_sock = init_socket(net_arg);
    printf("Ip: %s, port: %d\n", net_arg->address, net_arg->port);
    epfd = init_epoll(listen_sock);

    for (;;) {
        nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }
        for (i = 0; i < nfds; ++i) {
            if (events[i].data.fd == listen_sock) {
                while ((connfd = accept(listen_sock,
                                (struct sockaddr *)&client_addr,
                                &client_addrlen)) > 0) {
                    setnonblocking(connfd);
                    client = (_client_info *)malloc(sizeof(_client_info));
                    client->fd = connfd;
                    client->client_addr = client_addr;
                    /*ev.data.fd = connfd;*/
                    ev.data.ptr = (void *)client;
                    ev.events = EPOLLIN | EPOLLET;
                    if (epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev) < 0) {
                        /*TODO: debug mode and error handle*/
                        perror("epoll_ctl: add");
                        exit(EXIT_FAILURE);
                    }
                }
                if (connfd == -1 && errno != EAGAIN
                        && errno != EWOULDBLOCK && errno != ECONNABORTED
                        && errno != EPROTO && errno != EINTR) {
                    perror("accept");
                    /*TODO: close listen_sock and re-listen*/
                }
            } else if (events[i].events & EPOLLIN) {
                /*TODO: remeber to delete event and close fd*/
                client = (_client_info *)events[i].data.ptr;
                /*if ((sockfd = events[i].data.fd) < 0) {*/
                if (client->fd < 0) {
                    continue;
                }
                handle_request(client);
            } else if (events[i].events & EPOLLOUT) {
            }
        }
    }
}

void
handle_request(_client_info *client) {
    _ele element;
    _task *task = (_task *)malloc(sizeof(_task));
    task->fd = client->fd;
    task->client_addr = client->client_addr;
    /*free(client);*/
    element.task = task;
    while (q_add(TASK, element) != 0) {
        printf("There is no enough for new task!\n");
        sleep(1);
    }
    /*q_print(TASK);*/
    pthread_cond_broadcast(&task_available);
}