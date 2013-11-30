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
void str_cli(int sockfd);

int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in servaddr;

    if (argc != 2) 
        handle_err("usage: client <IP address>");

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
        handle_err("socket error");

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port = htons(SERV_PORT);

    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
        handle_err("connecting error");

    str_cli(sockfd); /* read from stdin and write to sockfd */

    close(sockfd);
    exit(EXIT_SUCCESS);
}

void handle_err(char *str)
{
    perror(str);
    exit(EXIT_FAILURE);
}

void str_cli(int sockfd)
{
    int n, m;
    char sendline[MAXLINE], recvline[MAXLINE];

    while ((n = read(STDIN_FILENO, sendline, MAXLINE)) > 0) {
        if (write(sockfd, sendline, n) == EOF) {
            perror("write error to server");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < 2; i++) {
            if ((m = read(sockfd, recvline, MAXLINE)) > 0)
                write(STDOUT_FILENO, recvline, n);
        }
    }
}