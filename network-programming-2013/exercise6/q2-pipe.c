#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

#define MEM_SIZE 1

void err_quit(char *str);

int main()
{
    int pipefd[2];
    char *ptr;
    int n, sum, val;

    if (pipe(pipefd) < 0) {
        err_quit("pipe error");
    }

   // nonblocking I/O
    val = fcntl(pipefd[1], F_GETFL, 0);
    fcntl(pipefd[1], F_SETFL, val | O_NONBLOCK);

    n = MEM_SIZE;
    sum = 0;
    // write to the socket
    for ( ; ; ) {
        if ((ptr = (char *) malloc(n)) == NULL) {
            err_quit("malloc error");
        }

        if (write(pipefd[1], ptr, n) < 0) {
            if (errno == EWOULDBLOCK) {
                printf("Max size: %d\n", sum);   
            }
            free(ptr);
            break;
        }
        
        sum += n;
        free(ptr);
    }

    exit(EXIT_SUCCESS);
}

void err_quit(char *str)
{
    perror(str);
    exit(EXIT_FAILURE);
}