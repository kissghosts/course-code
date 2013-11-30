#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

#define MAXLINE 10240
#define SERV_PORT 50011

void handle_err(char *str);
void str_recv(int sockfd, struct sockaddr *pcliaddr, socklen_t clilen);

int main(int argc, char const *argv[])
{
    int sockfd;
    struct sockaddr_in cliaddr, servaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1)
        handle_err("socket error");

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 )
        handle_err("binding error");

    /* process the request */
    str_recv(sockfd, (struct sockaddr *) &cliaddr, sizeof(cliaddr));

    exit(EXIT_SUCCESS);
}

void handle_err(char *str)
{
    perror(str);
    exit(EXIT_FAILURE);
}

void str_recv(int sockfd, struct sockaddr *pcliaddr, socklen_t clilen)
{
    ssize_t n;
    socklen_t len;
    char buf[MAXLINE];

    for ( ; ; ) {
        len = clilen;
        while ((n = recvfrom(sockfd, buf, MAXLINE, 0, pcliaddr, &len)) > 0) {
            write(STDOUT_FILENO, buf, n);
        }

        if (n < 0)
            handle_err("read error");
    }
}