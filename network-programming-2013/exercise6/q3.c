#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <strings.h>

#define SERV_PORT 50011

void handle_err(char *str);

int main(int argc, char *argv[])
{
    int sockfd, mss, val;
    socklen_t optlen;
    struct sockaddr_in servaddr;

    if (argc != 2) 
        handle_err("usage: client <IP address>");

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
        handle_err("socket error");

    optlen = sizeof(mss);
    val = getsockopt(sockfd, IPPROTO_TCP, TCP_MAXSEG, &mss, &optlen);
    if (val == -1) {
        handle_err("getsockopt error");
    } else {
        printf("mss = %d\n", mss);
    }

    mss = 256;
    val = setsockopt(sockfd, IPPROTO_TCP, TCP_MAXSEG, &mss, sizeof(mss));
    if (val == -1) {
        handle_err("getsockopt error");
    } else {
        printf("set mss to %d\n", mss);
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port = htons(SERV_PORT);

    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
        handle_err("connecting error");

    val = getsockopt(sockfd, IPPROTO_TCP, TCP_MAXSEG, &mss, &optlen);
    if (val == -1) {
        handle_err("getsockopt error");
    } else {
        printf("mss = %d\n", mss);
    }

    close(sockfd);
    exit(EXIT_SUCCESS);
}

void handle_err(char *str)
{
    perror(str);
    exit(EXIT_FAILURE);
}