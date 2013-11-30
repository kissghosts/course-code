#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <ctype.h>
#include <sys/sendfile.h>

#define MAXLINE 2048
#define LISTEN_BACKLOG 10
#define MAXLEN 10240

struct addrinfo *rp; /* global variable */

void handle_err(char *str);
void str_cli(int sockfd);
void print_ipaddr(struct sockaddr_in *servaddr);
void sig_chld(int signo);
void sig_int(int signo);
void server(int port);
void response(int sockfd);
char *get_file_size(char *path);
void file_transfer(char *path, char *port);

int main(int argc, char *argv[])
{
    int sockfd, status;
    struct addrinfo hints, *result;

    if (argc < 3) 
        handle_err("usage: client <url> <service>");

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;    /* Allow IPv4 */
    hints.ai_socktype = SOCK_STREAM; /* stream socket */

    /* initialize socket fd */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
        handle_err("socket error");

    /* get ip address and connect to it*/
    status = getaddrinfo(argv[1], argv[2], &hints, &result);
    if (status != 0)
        handle_err("getaddrinfo error");

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        fflush(stdin);
        print_ipaddr((struct sockaddr_in *)rp->ai_addr);
        if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) == 0) {
            break;
        }
    }

    if (rp == NULL)
        handle_err("Could not find correct ip to connect");

    str_cli(sockfd); /* client */

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
    int n, m, i, j, k;
    char recvline[MAXLINE], sendline[MAXLINE];

    if ((n = read(sockfd, recvline, MAXLINE)) > 0) {
        write(STDOUT_FILENO, recvline, n);
    }

    usleep(10000);
    if (write(sockfd, "hello\n", 6) == EOF)
        handle_err("write error to server");

    if ((m = read(sockfd, recvline, MAXLINE)) > 0)
        write(STDOUT_FILENO, recvline, m);

    usleep(30000);
    if (write(sockfd, "yfliu\n", 6) == EOF)
        handle_err("write error to server");

    if ((m = read(sockfd, recvline, MAXLINE)) > 0)
        write(STDOUT_FILENO, recvline, m);

    usleep(800000);
    if (write(sockfd, "ok\n", 10) == EOF)
        handle_err("write error to server");

    if ((m = read(sockfd, recvline, MAXLINE)) > 0)
        write(STDOUT_FILENO, recvline, m);

    if ((m = read(sockfd, recvline, MAXLINE)) > 0)
        write(STDOUT_FILENO, recvline, m);

    for (i = m - 2; i > 0; i--) {
        if (recvline[i] == ' ')
            break;
    }

    k = m - 1 - i;
    char port[m - 1 - i];
    for (j = 0; j < k - 1; j++) {
        port[j] = recvline[++i];
    }
    port[j] = '\0';

    server(atoi(port));
}

void print_ipaddr(struct sockaddr_in *servaddr)
{
    char ipstr[INET_ADDRSTRLEN];
    void *addr;

    addr = &(servaddr->sin_addr);
    inet_ntop(AF_INET, addr, ipstr, sizeof(ipstr));
    printf("Connecting to %s\n", ipstr);
}

void server(int port)
{
    int listenfd, connfd;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    struct sigaction act, oact;
    act.sa_handler = sig_int;
    act.sa_flags = SA_RESETHAND;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1)
        handle_err("socket error");

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if (bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 )
        handle_err("binding error");

    if (listen(listenfd, LISTEN_BACKLOG) == -1)
        handle_err("listen error");

    if (sigaction(SIGINT, &act, &oact) < 0) {
        handle_err("sigint error");
    }

    /* call accept, wait for a client */ 
    for ( ; ; ) {
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen); 
        if (connfd == -1)
            handle_err("accept error");

        if ((childpid = fork()) < 0)
            handle_err("fork error");
        else if (childpid == 0) { /* child process */
            close(listenfd);
            response(connfd); /* process the request */
            exit(EXIT_SUCCESS);
        }

        close(connfd); /* parent closes connected socket */
    }
}

void response(int sockfd)
{
    ssize_t n;
    char buf[MAXLINE];
    char *file_path = "Courses/network_programming/break/client.c\n";
    char *ptr = buf;
    char *token, *path, *port;
    char *split = " ";
    int byte;
    int name_len = strlen(file_path);

    while ((n = read(sockfd, buf, MAXLINE)) > 0) {
        if (buf[0] == '#') {
            write(STDOUT_FILENO, buf, n);
        } else if (buf[0] == 'E') {
            write(STDOUT_FILENO, "E received\n", 12);
            if (buf[n - 1] != '\n') {
                ptr = ptr + n;
                byte = n;
                while (byte < MAXLINE 
                    && (n = read(sockfd, ptr, MAXLINE - byte)) > 0) {
                    byte = byte + n;
                    ptr = ptr + n;
                    if (*(ptr - 1) == '\n') {
                        break;
                    }
                }
            }

            for (int i = 0; i < byte - 1; i++) {
                buf[i] = toupper(buf[i]);
            }

            if (write(sockfd, buf, byte) == EOF)
                handle_err("write error to server");
        } else if (buf[0] == 'C') {
            write(STDOUT_FILENO, "C received\n", 12);
            if (write(sockfd, file_path, name_len) == EOF)
                handle_err("write error to server");
        } else if (buf[0] == 'F') {
            write(STDOUT_FILENO, "F received\n", 12);
            write(STDOUT_FILENO, buf, n);
            token = strtok(buf, split);
            path = strtok(NULL, split);
            port = strtok(NULL, split);
            if (path == NULL || port == NULL) {
                handle_err("invalid F command");
            }

            if (port[strlen(port) - 1] == '\n') {
                port[strlen(port) - 1] = '\0';
            }

            char *file_size = get_file_size(path);
            if (write(sockfd, file_size, strlen(file_size)) == EOF)
                handle_err("write error to server");
            if (write(sockfd, "\n", 1) == EOF)
                handle_err("write error to server");

            file_transfer(path, port);
        } else if (buf[0] == 'A') {
            write(STDOUT_FILENO, "A received\n", 12);
            write(STDOUT_FILENO, buf, n);
        } else if (buf[0] == 'Q') {
            write(STDOUT_FILENO, "Q received\n", 12);
            write(STDOUT_FILENO, buf, n);
            close(sockfd);
            return;
        } else {
            write(STDOUT_FILENO, "other condition\n", 20);
            write(STDOUT_FILENO, buf, n);
        }
    }

    if (n < 0)
        handle_err("read error");
}

void sig_int(int signo)
{
    printf("terminated by signal %d\n", signo);
    return;
}

char *get_file_size(char *path)
{
    struct stat sb;
    int n;

    if (stat(path, &sb) == -1) {
        handle_err("stat error");
    }

    n = snprintf(NULL, 0, "%lld", (long long)sb.st_size);
    if (n <= 0) {
        handle_err("long to string error");
    }
    char buf[n + 1];
    snprintf(buf, n + 1, "%lld", (long long)sb.st_size);
    return buf;
}

void file_transfer(char *path, char *port)
{
    int sockfd, fd;
    struct sockaddr_in servaddr;
    struct stat sb;
    long long len;
    int n = MAXLEN;
    int m;
    char ipstr[INET_ADDRSTRLEN];
    void *addr;
    struct sockaddr_in *ipaddr = (struct sockaddr_in *)rp->ai_addr;
    char buf[MAXLINE];

    addr = &(ipaddr->sin_addr);
    inet_ntop(AF_INET, addr, ipstr, sizeof(ipstr));

    if (stat(path, &sb) == -1) {
        handle_err("stat error");
    }

    len = (long long)sb.st_size;

    /* initialize socket fd */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
        handle_err("socket error");

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(ipstr);
    servaddr.sin_port = htons(atoi(port));

    printf("connecting to %s: %s\n", ipstr, port);

    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        handle_err("connecting error");
    }
    
    fd = open(path, O_RDONLY);
    if (fd < 0) {
        handle_err("open file error");
    }

    // do {
    //     if (len < (long long)n) {
    //         n = (int)len;
    //     }

    //     if ((m = sendfile(sockfd, fd, NULL, n)) == -1) {
    //         handle_err("sendfile error");
    //     }
    //     len -= (long long)n;
    //     printf("%d\n", m);
    // } while (len > 0);

    if ((m = sendfile(sockfd, fd, NULL, sb.st_size)) == -1) {
        handle_err("sendfile error");
    }

    printf("file transfered\n");
    close(sockfd);
}