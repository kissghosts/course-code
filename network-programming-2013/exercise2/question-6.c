#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define MAXLINE 1024

void err_quit(char *str);
void doubler(int fd);

int main()
{
    pid_t pid;
    int pipefd[2];
    char line[MAXLINE];

    if (pipe(pipefd) < 0)
        err_quit("pipe error");

    if ((pid = fork()) < 0) {
        err_quit("fork error");
    } else if (pid > 0) { /* parent */
        close(pipefd[0]); /* close read end */

        // read from stdin and write to pipe
        int n;
        while ((n = read(STDIN_FILENO, line, MAXLINE)) > 0) {
            if (write(pipefd[1], line, n) == EOF)
                err_quit("write error to pipe\n");            
        }

        // close write for reader
        close(pipefd[1]);

        if (waitpid(pid, NULL, 0) < 0)
            err_quit("waitpid error");

        exit(0);
    } else { /* child */
        close(pipefd[1]); /* close write end */

        doubler(pipefd[0]);
        _exit(0);
    }

    exit(0);
}

void err_quit(char *str)
{
    fprintf(stderr, "%s\n", str);
    exit(1);
}

void doubler(int fd)
{
    int n;
    char buf[MAXLINE];

    while ((n = read(fd, buf, MAXLINE)) > 0) {
        for (int i = 0; i < 2; i++) {
            if (write(STDOUT_FILENO, buf, n) == EOF)
                err_quit("write error to stdout");
        }
    }
}