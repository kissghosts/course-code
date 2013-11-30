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

#define SERV_PORT 50011
#define MAXBYTES 256

void handle_err(char *str);
void connect_to_srv(char *ipaddr);

int main(int argc, char *argv[])
{
    int i;

    if (argc != 3)
        handle_err("usage: a.out <IPaddress> <Num>");

    for (i = 0; i < atoi(argv[2]); i++) {
        printf("connecting %d\n", i);
        connect_to_srv(argv[1]);
    }

    exit(EXIT_SUCCESS);
}

void handle_err(char *str)
{
    perror(str);
    exit(EXIT_FAILURE);
}

void connect_to_srv(char *ipaddr)
{
    int sockfd, count;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
        handle_err("socket error");

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ipaddr);
    servaddr.sin_port = htons(SERV_PORT);

    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
        handle_err("connecting error");

    close(sockfd);
}