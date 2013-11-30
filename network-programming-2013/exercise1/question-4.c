#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

#define BUFFSIZE 4096
#define MAXLINES 4096

void print(int file); /* write the input to stdout*/
void sig_handler(int sig); /* signal handler */

// use system calls read, write and open

int main(int argc, char *argv[])
{
    char err1[30] = "losing args\n";
    char err2[30] = "fail to use handler\n";
    char err3[30] = "can't open your input file\n";
    char cmp_str[3] = "-C";
    int file; /* file descriptor */

    if (argc == 1) {
        write(STDERR_FILENO, err1, 30);
        exit(1);
    }

    if ((argc == 2) && !strcmp(argv[1], cmp_str)) {
        write(STDERR_FILENO, err1, 30);
        exit(1);
    }

    for (int i = 1; argv[i] != NULL; i++) {
        if (!strcmp(argv[i], cmp_str)) {
            if (signal(SIGINT, sig_handler) == SIG_ERR) {
                write(STDERR_FILENO, err2, 30);
                exit(1);
            }
        }
    }

    for (int j = 1; argv[j] != NULL; j++) {
        if (strcmp(argv[j], cmp_str)) {
            if ((file = open(argv[j], O_RDONLY)) == -1) {
                write(STDERR_FILENO, err3, 30);
                exit(1);
            } else 
                print(file);
        }
    }

    return 0;
}

void print(int file) {
    int n;
    char buf[BUFFSIZE];
    char buf_err[30] = "can't write to stdout\n";

    while ((n = read(file, buf, BUFFSIZE)) > 0) {
        if (write(STDOUT_FILENO, buf, n) != n) {
            write(STDERR_FILENO, buf_err, 30);
            exit(1);
        }
    }

    sleep(10);
}

void sig_handler(int sig) {
    char buf_err[30] = "catch SIGINT\n";

    if (sig == SIGINT) {
        write(STDERR_FILENO, buf_err, 30);
        exit(1);
    }
}