#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define PATH "doubler"
#define MAXLINE 1024
extern char **environ;

void err_quit(char *str);
void pr_exit(int status);

int main()
{
    pid_t pid;
    int pipefd[2];
    char line[MAXLINE];
    int status; /* return exit status of child process */
    char *argv[] = {NULL};

    if (pipe(pipefd) < 0)
        err_quit("pipe error");

    if ((pid = fork()) < 0) {
        err_quit("fork error");
    } else if (pid == 0) { /* child */
        close(pipefd[1]); /* close write end */
        if (pipefd[0] != STDIN_FILENO) {
            if (dup2(pipefd[0], STDIN_FILENO) != STDIN_FILENO)
                err_quit("dup2 error");
            close(pipefd[0]); /* dont need this after dup2 */
        }

        // exec doubler
        if (execve(PATH, argv, environ) < 0)
            err_quit("exec doubler error");
        
        _exit(EXIT_SUCCESS);
    } else { /* parent */
        close(pipefd[0]); /* close read end */

        // read from stdin and write to pipe
        int n;
        while ((n = read(STDIN_FILENO, line, MAXLINE)) > 0) {
            if (write(pipefd[1], line, n) == EOF)
                err_quit("write error to pipe\n");            
        }

        // close write for reader
        close(pipefd[1]);

        if (waitpid(pid, &status, 0) < 0) {
            err_quit("waitpid error");
        }
    }

    pr_exit(status);
    exit(EXIT_SUCCESS);
}

void pr_exit(int status)
{
    if (WIFEXITED(status)) {
        printf("normally exited, status=%d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
       printf("killed by signal %d\n", WTERMSIG(status));
    } else if (WIFSTOPPED(status)) {
       printf("stopped by signal %d\n", WSTOPSIG(status));
    } else if (WIFCONTINUED(status)) {
       printf("continued\n");
    }
}

void err_quit(char *str)
{
    perror(str);
    exit(EXIT_FAILURE);
}