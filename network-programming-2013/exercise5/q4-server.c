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

#define MAXBYTES 256
#define LISTEN_BACKLOG 5
#define SERV_PORT 50011

void handle_err(char *str);
void str_write(int sockfd);
void sig_int(int signo);

int main(int argc, char const *argv[])
{
    int listenfd, connfd;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    struct sigaction act, oact;
    act.sa_handler = sig_int;
    act.sa_flags = SA_RESETHAND;

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

    if (sigaction(SIGINT, &act, &oact) < 0) {
        handle_err("sigaction error");
    }

    /* call accept, wait for a client */ 
    for ( ; ; ) {
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen); 
        if (connfd == -1)
            handle_err("accept error");

        if (shutdown(connfd, SHUT_RDWR) < 0)
            handle_err("shutdown error");

        if ((childpid = fork()) < 0)
            handle_err("fork error");
        else if (childpid == 0) { /* child process */
            close(listenfd);
            str_write(connfd); /* process the request */
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

void str_write(int sockfd)
{
    ssize_t n;
    char buf[MAXBYTES];

    for ( ; ; ) {
        while ((n = read(sockfd, buf, MAXBYTES)) > 0) {
            write(STDOUT_FILENO, buf, n);
        }

        if (n < 0)
            handle_err("read error");
    }
}

void sig_int(int signo)
{
    printf("terminated by signal %d\n", signo);
    exit(0);
}