#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <strings.h>

#define SERV_PORT 50011

void handle_err(char *str);

int main(int argc, char *argv[])
{
    int sockfd, sendbuf, val;
    socklen_t optlen;
    struct sockaddr_in servaddr;

    if (argc != 2) 
        handle_err("usage: client <IP address>");

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
        handle_err("socket error");

    optlen = sizeof(sendbuf);
    val = getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sendbuf, &optlen);
    if (val == -1) {
        handle_err("getsockopt error");
    } else {
        printf("send buffer size = %d\n", sendbuf);
    }

    sendbuf = 50000;
    val = setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sendbuf, optlen);
    if (val == -1) {
        handle_err("setsockopt error");
    }
    printf("set send buffer size to %d\n", sendbuf);
    
    val = getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sendbuf, &optlen);
    if (val == -1) {
        handle_err("getsockopt error");
    } else {
        printf("send buffer size = %d\n", sendbuf);
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port = htons(SERV_PORT);

    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
        handle_err("connecting error");

    val = getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sendbuf, &optlen);
    if (val == -1) {
        handle_err("getsockopt error");
    } else {
        printf("send buffer size = %d\n", sendbuf);
    }

    close(sockfd);
    exit(EXIT_SUCCESS);
}

void handle_err(char *str)
{
    perror(str);
    exit(EXIT_FAILURE);
}