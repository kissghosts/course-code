#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#ifndef MAXLINE
#define MAXLINE 1024
#endif

int main()
{
    int n;
    char buf[MAXLINE];

    while ((n = read(STDIN_FILENO, buf, MAXLINE)) > 0) {
        for (int i = 0; i < 2; i++) {
            if (write(STDOUT_FILENO, buf, n) == EOF) {
                perror("write error to stdout");
                exit(EXIT_FAILURE);
            }
        }
    }
    
    exit(EXIT_SUCCESS);
}