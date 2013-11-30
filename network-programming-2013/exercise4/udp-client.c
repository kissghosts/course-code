#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

#define MAXLINE 256
#define SERV_PORT 50011

void handle_err(char *str);
void str_cli(int sockfd, const struct sockaddr *pservaddr, socklen_t servlen);

int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in servaddr;

    if (argc != 2) 
        handle_err("usage: client <IP address>");

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1)
        handle_err("socket error");

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    if (inet_aton(argv[1], &servaddr.sin_addr) == 0)
        handle_err("error IP address");

    str_cli(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    close(sockfd);
    exit(EXIT_SUCCESS);
}

void handle_err(char *str)
{
    perror(str);
    exit(EXIT_FAILURE);
}

void str_cli(int sockfd,  const struct sockaddr *pservaddr, socklen_t servlen)
{
    int n, m;
    char sendline[MAXLINE], recvline[MAXLINE + 1];

    while ((n = read(STDIN_FILENO, sendline, MAXLINE)) > 0) {
        sendto(sockfd, sendline, n, 0, pservaddr, servlen);

        for (int i = 0; i < 2; i++) {
            if ((m = recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL)) > 0) {
                recvline[n] = 0;
                write(STDOUT_FILENO, recvline, n);
            }
        }
    }
}