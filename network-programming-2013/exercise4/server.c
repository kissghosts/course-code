#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/time.h>

#define LISTEN_BACKLOG 5
#define SERV_PORT 50011

void handle_err(char *str);
void read_data(int sockfd, int count);
void sig_chld(int signo);

int main(int argc, char const *argv[])
{
    int listenfd, connfd;
    int count;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    struct sigaction act, oact;
    act.sa_handler = sig_chld;
    act.sa_flags = SA_RESETHAND;

    if (argc != 2) {
        handle_err("usage: server <count>");
    }

    count = atoi(argv[1]);

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1)
        handle_err("socket error");

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    if (bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 )
        handle_err("binding error");

    if (listen(listenfd, LISTEN_BACKLOG) == -1)
        handle_err("listen error");

    if (sigaction(SIGCHLD, &act, &oact) < 0) {
        handle_err("waitpid error");
    }

    /* call accept, wait for a client */ 
    for ( ; ; ) {
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen); 
        if (connfd == -1)
            handle_err("accept error");

        if ((childpid = fork()) < 0)
            handle_err("fork error");
        else if (childpid == 0) { /* child process */
            close(listenfd);
            read_data(connfd, count); /* process the request */
            exit(0);
        }

        close(connfd); /* parent closes connected socket */
    }

    exit(EXIT_SUCCESS);
}

void handle_err(char *str)
{
    perror(str);
    exit(EXIT_FAILURE);
}

void read_data(int sockfd, int count)
{
    ssize_t n;
    char buf[count];
    struct timeval start;
    struct timeval end;
    float sec, usec, diff;
    int flag = 0;

    for ( ; ; ) {
        while ((n = read(sockfd, buf, count)) > 0) {
            if (n > 0 && flag == 0) {
                // gettimeofday from <sys/time.h>
                gettimeofday(&start, NULL);
                flag = 1;
            }
        }

        if (n == 0 && flag == 1) {
            gettimeofday(&end, NULL);
            sec = end.tv_sec - start.tv_sec;
            usec = end.tv_usec - start.tv_usec;
            diff = sec + usec / 1000000;
            printf("The elapsed time for receiving is %fs\n", diff);
            flag = 0;
        }

        if (n < 0)
            handle_err("read error");
    }
}

void sig_chld(int signo)
{
    pid_t pid;
    int stat;

    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0) {
        printf("child %d terminated\n", pid);
    }

    return;
}