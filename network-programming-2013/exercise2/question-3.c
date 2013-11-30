#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

int do_child(char *path, char *arg);

extern char **environ;

int main(int argc, char *argv[])
{
    pid_t pid;
    struct stat buf;

    if (argc < 2) {
        fprintf(stderr, "input args error\n");
        exit(1);
    }

    // stat() in <sys/stat.h>, check the path given
    if (stat(argv[1], &buf) == -1) {
        fprintf(stderr, "error path, fail to check the file\n");
        exit(1);
    }

    if (!S_ISREG(buf.st_mode)) {
        fprintf(stderr, "error file type, should be regular file\n");
        exit(1);
    }

    if ((pid = fork()) < 0) {
        fprintf(stderr, "[pid %d] fork error\n", getpid());
        exit(1);
    } else if (pid == 0) { /* child */
        do_child(argv[1], argv[2]);
        _exit(0);
    } else { /* parent */
        if (waitpid(pid, NULL, 0) < 0) {
            printf("[pid %d] wait error\n", getpid());
            exit(1);
        }

        for (int i = 0; i < 5; i++) {
            printf("[pid %d] i = %d\n", getpid(), i);
        }
        sleep(1);
    }

    exit(0);
}

int do_child(char *path, char *arg) 
{
    printf("%s\n", path);
    printf("%s\n", arg);

    if (execle(path, arg, (char *)0, environ) < 0) {
        fprintf(stderr, "[pid %d] execl error\n", getpid());
        _exit(1);
    }
    exit(0);
}
