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

void handle_err(char *str);
void send_data(int sockfd, int count, int bytes);
void elapsed_time(struct timeval start, struct timeval end, char *str);

int main(int argc, char *argv[])
{
    int sockfd, count;
    struct sockaddr_in servaddr;
    size_t total_bytes, each_bytes;
    struct timeval start1, start2;
    struct timeval end1, end2;

    if (argc != 4) 
        handle_err("usage: client <IP address> <total B> <eachline B>");

    total_bytes = atoi(argv[2]);
    each_bytes = atoi(argv[3]);

    if (total_bytes < each_bytes) {
        handle_err("usage: total bytes must not be less than single ones");
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
        handle_err("socket error");

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port = htons(SERV_PORT);

    // gettimeofday from <sys/time.h>
    gettimeofday(&start1, NULL);
    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
        handle_err("connecting error");
    gettimeofday(&end1, NULL);
    // calculating the time
    elapsed_time(start1, end1, "establising the connection");
    
    // calculating nums of calling write()
    count = total_bytes / each_bytes;
    if ((total_bytes % each_bytes) != 0) {
        count += 1;
    }

    gettimeofday(&start2, NULL);
    send_data(sockfd, count, each_bytes); /* send random data to sockfd */
    gettimeofday(&end2, NULL);
    elapsed_time(start2, end2, "sending data");

    close(sockfd);
    exit(EXIT_SUCCESS);
}

void handle_err(char *str)
{
    perror(str);
    exit(EXIT_FAILURE);
}

void send_data(int sockfd, int count, int bytes)
{
    int i;
    char buf[bytes]; /* random data for every write cmd*/

    for (i = 0; i < count; i++) {
        if (write(sockfd, buf, bytes) == EOF) {
            handle_err("write error to server");
        }
    }
}

void elapsed_time(struct timeval start, struct timeval end, char *str) {
    float sec, usec, diff;

    sec = end.tv_sec - start.tv_sec;
    usec = end.tv_usec - start.tv_usec;
    diff = sec + usec / 1000000;
    printf("The elapsed time for %s is %fs\n", str, diff);    
}
