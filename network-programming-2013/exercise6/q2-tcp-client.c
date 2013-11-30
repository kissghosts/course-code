#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <strings.h>
#include <fcntl.h>
#include <errno.h>

#define SERV_PORT 50011
#define MEM_SIZE 1

void handle_err(char *str);
void cli_str(int sockfd);

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
    int n, sum, val;
    char *ptr;
    n = MEM_SIZE;
    sum = 0;
    
    // nonblocking I/O
    val = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, val | O_NONBLOCK);
    
    // write to the socket
    for ( ; ; ) {
        if ((ptr = (char *) malloc(n)) == NULL) {
            handle_err("malloc error");
        }

        if (write(sockfd, ptr, n) < 0) {
            if (errno == EWOULDBLOCK) {
                printf("Max size: %d\n", sum);   
            }
            free(ptr);
            break;
        }
        sum += n;
        free(ptr);
    }
}