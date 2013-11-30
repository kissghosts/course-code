#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#define SERV_PORT 50011
#define MAXBYTES 256

void handle_err(char *str);
void cli_str(int sockfd);

int main(int argc, char *argv[])
{
    int sockfd, count;
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
    
    cli_str(sockfd);
    close(sockfd);
    exit(EXIT_SUCCESS);
}

void handle_err(char *str)
{
    perror(str);
    exit(EXIT_FAILURE);
}

void cli_str(int sockfd)
{
    int n, m;
    char sendline[MAXBYTES], recvline[MAXBYTES];

    while ((n = read(STDIN_FILENO, sendline, MAXBYTES)) > 0) {
        if (write(sockfd, sendline, n) == EOF) {
            handle_err("write error to server");
        }

        if ((m = read(sockfd, recvline, MAXBYTES)) > 0)
            write(STDOUT_FILENO, recvline, n);
    }
}