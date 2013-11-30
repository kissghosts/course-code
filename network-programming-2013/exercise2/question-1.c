#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

void waitchld(int signo);

int main()
{
    struct sigaction act, oact;
    act.sa_handler = waitchld;
    act.sa_flags = SA_RESETHAND;

    if (sigaction(SIGCHLD, &act, &oact) < 0) {
        /* error */
    }

    if ( pid = fork()) < 0) {
        /* fork failed*/ 
    } else if (pid >  0) {
        /* do the parent loop */
    } else { 
        /* do the child’s job */
        _exit(0); 
    }

    exit(0);
}

void waitchld(int signo)
{
    pid_t pid;
    int stat;
    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0) {
        /* examine the exit status at will …*/ 
    }
    exit(0);
}