#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>

#define PATH "converter"
#define FIFO1 "/tmp/fifo1"
#define FIFO2 "/tmp/fifo2"
#define MAXLINE 1024
extern char **environ;

void err_quit(char *str);

int main()
{
    pid_t pid;
    int rfd, wfd;
    char line[MAXLINE];
    char *argv[] = {NULL};

    if (mkfifo(FIFO1, 0666) < 0)
        err_quit("fifo error");

    if (mkfifo(FIFO2, 0666) < 0) {
        unlink(FIFO1);
        err_quit("fifo error");
    }

    if ((pid = fork()) < 0) {
        err_quit("fork error");
    } else if (pid > 0) { /* parent */
        if ((wfd = open(FIFO1, O_WRONLY)) < 0)
            err_quit("parent: open fifo error");

        if ((rfd = open(FIFO2, O_RDONLY)) < 0)
            err_quit("parent: open fifo error");

        // read from stdin and write to fifo
        int n;
        if ((n = read(STDIN_FILENO, line, MAXLINE)) > 0) {
            if (write(wfd, line, n) == EOF)
                err_quit("parent: write error to fifo\n");            
        }

        // wait child process
        if (waitpid(pid, NULL, 0) < 0) {
            err_quit("parent: waitpid error");
        }

        if ((n = read(rfd, line, MAXLINE)) > 0) {
            if (write(STDOUT_FILENO, line, n) == EOF)
                err_quit("parent: write error to stdout\n");            
        }

        if (close(rfd) < 0 || close(wfd) < 0)
            err_quit("parent: close fifo error");
        if (unlink(FIFO1) < 0 || unlink(FIFO2) < 0)
            err_quit("parent: unlink fifo error");
    } else { /* child */
        if ((rfd = open(FIFO1, O_RDONLY)) < 0)
            err_quit("child: open fifo error");

        if ((wfd = open(FIFO2, O_WRONLY)) < 0)
            err_quit("child: open fifo error");
        
        // redirect stdin and stdout to fifos
        if (rfd != STDIN_FILENO) {
            if (dup2(rfd, STDIN_FILENO) != STDIN_FILENO)
                err_quit("child: dup2 error");
            close(rfd); /* dont need this after dup2 */
        }

        if (wfd != STDOUT_FILENO) {
            if (dup2(wfd, STDOUT_FILENO) != STDOUT_FILENO)
                err_quit("child: dup2 error");
            close(wfd); /* dont need this after dup2 */
        }

        // exec converter
        if (execve(PATH, argv, environ) < 0)
            err_quit("child: exec converter error");

        _exit(EXIT_SUCCESS);
    }

    exit(EXIT_SUCCESS);
}

void err_quit(char *str)
{
    perror(str);
    exit(EXIT_FAILURE);
}