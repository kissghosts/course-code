#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define PATH "doubler"
extern char **environ;

void err_quit(char *str);
void pr_exit(int status);

int main()
{
    pid_t pid;
    int status; /* return exit status of child process */
    char *argv[] = {NULL};

    if ((pid = fork()) < 0) {
        err_quit("fork error");
    } else if (pid == 0) { /* child */
        // exec doubler
        if (execve(PATH, argv, environ) < 0)
            err_quit("exec doubler error");
        
        _exit(EXIT_SUCCESS);
    } else { /* parent */
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