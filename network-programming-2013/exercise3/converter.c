#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>

#ifndef MAXLINE
#define MAXLINE 1024
#endif

int main()
{
    int n;
    char buf[MAXLINE];

    if ((n = read(STDIN_FILENO, buf, MAXLINE)) > 0) {
        // convert to lower case
        for (int i = 0; buf[i] != '\n'; i++)
            buf[i] = tolower(buf[i]);

        if (write(STDOUT_FILENO, buf, n) == EOF) {
            perror("write error to stdout");
            exit(EXIT_FAILURE);
        }
    }
    
    exit(EXIT_SUCCESS);
}