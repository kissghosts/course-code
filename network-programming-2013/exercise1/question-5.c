#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

#define BUFFSIZE 4096
#define MAXLINES 4096
#define ERRBUFFSIZE 30

void print(int file); /* write the input to stdout*/
void sig_handler(int sig); /* signal handler */

// use system calls read, write and open

int main(int argc, char *argv[])
{
    char err1[ERRBUFFSIZE] = "losing args\n";
    char err2[ERRBUFFSIZE] = "fail to use handler\n";
    char err3[ERRBUFFSIZE] = "can't open your input file\n";
    char cmp_str[3] = "-C";
    int file; /* file descriptor */

    struct sigaction act, oact;
    act.sa_handler = sig_handler;
    act.sa_flags = SA_RESETHAND;

    if (sigemptyset(&act.sa_mask) == -1) {
        write(STDERR_FILENO, err2, ERRBUFFSIZE);
        exit(1);
    }

    if (argc == 1 || (argc == 2 && !strcmp(argv[1], cmp_str))) {
        write(STDERR_FILENO, err1, ERRBUFFSIZE);
        exit(1);
    }

    for (int i = 1; argv[i] != NULL; i++) {
        if (!strcmp(argv[i], cmp_str)) {
            if (sigaction(SIGINT, &act, &oact) < 0) {
                write(STDERR_FILENO, err2, ERRBUFFSIZE);
                exit(1);
            }
        }
    }

    for (int j = 1; argv[j] != NULL; j++) {
        if (strcmp(argv[j], cmp_str)) {
            if ((file = open(argv[j], O_RDONLY)) == -1) {
                write(STDERR_FILENO, err3, ERRBUFFSIZE);
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
    char buf_err[ERRBUFFSIZE] = "can't write to stdout\n";

    while ((n = read(file, buf, BUFFSIZE)) > 0) {
        if (write(STDOUT_FILENO, buf, n) != n) {
            write(STDERR_FILENO, buf_err, ERRBUFFSIZE);
            exit(1);
        }
    }

    sleep(10);
}

void sig_handler(int sig) {
    char buf_err[ERRBUFFSIZE] = "catch SIGINT\n";

    if (sig == SIGINT) {
        write(STDERR_FILENO, buf_err, ERRBUFFSIZE);
        exit(1);
    }
}