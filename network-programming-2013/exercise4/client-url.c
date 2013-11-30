#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

#define MAXLINE 256

void handle_err(char *str);
void str_cli(int sockfd);
void print_ipaddr(struct sockaddr_in *servaddr);

int main(int argc, char *argv[])
{
    int sockfd, status;
    struct addrinfo hints, *result, *rp;

    if (argc < 3) 
        handle_err("usage: client <url> <service>");

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;    /* Allow IPv4 */
    hints.ai_socktype = SOCK_STREAM; /* stream socket */

    /* initialize socket fd */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
        handle_err("socket error");

    /* get ip address */
    status = getaddrinfo(argv[1], argv[2], &hints, &result);
    if (status != 0)
        handle_err("getaddrinfo error");

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        print_ipaddr((struct sockaddr_in *)rp->ai_addr);
        if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) == 0) {
            break;
        }
    }

    if (rp == NULL)
        handle_err("Could not find correct ip to connect");

    str_cli(sockfd); /* echo client */

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

        if ((m = read(sockfd, recvline, MAXLINE)) > 0)
            write(STDOUT_FILENO, recvline, n);
    }
}

void print_ipaddr(struct sockaddr_in *servaddr)
{
    char ipstr[INET_ADDRSTRLEN];
    void *addr;
    char *ipver;

    addr = &(servaddr->sin_addr);
    inet_ntop(AF_INET, addr, ipstr, sizeof(ipstr));
    printf("Connecting to %s\n", ipstr);
}