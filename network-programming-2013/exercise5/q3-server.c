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

#define MAXBYTES 256
#define LISTEN_BACKLOG 5
#define SERV_PORT 50011

void handle_err(char *str);
void str_echo(int sockfd);
void sig_chld(int signo);
void sig_int(int signo);

int main(int argc, char const *argv[])
{
    int listenfd, connfd;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    struct sigaction act1, oact1, act2, oact2;
    act1.sa_handler = sig_chld;
    act1.sa_flags = SA_RESETHAND;
    act2.sa_handler = sig_int;
    act2.sa_flags = SA_RESETHAND;

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

    if (sigaction(SIGCHLD, &act1, &oact1) < 0) {
        handle_err("waitpid error");
    }
    if (sigaction(SIGINT, &act2, &oact2) < 0) {
        handle_err("sigint error");
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
            str_echo(connfd); /* process the request */
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

void str_echo(int sockfd)
{
    ssize_t n;
    char buf[MAXBYTES];

    for ( ; ; ) {
        while ((n = read(sockfd, buf, MAXBYTES)) > 0) {
            write(sockfd, buf, n);
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

    exit(0);
}

void sig_int(int signo)
{
    printf("terminated by signal %d\n", signo);
    exit(0);
}