#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define FORKNUM 5 /* count num to call fork */

int main(int argc, char *argv[])
{
    pid_t pid;
    int pnum, loop_num;
    pnum = FORKNUM;

    if (argc == 2) {
        // convert str to int num
        if ((loop_num = atoi(argv[1])) <= 0) {
            fprintf(stderr, "wrong arg given: %s\n", argv[1]);
            exit(1);
        }
    } else {
        loop_num = 10; /* set default */
    }

    while (pnum > 0) {
        if ((pid = fork()) < 0) {
            fprintf(stderr, "[pid %d] fork error\n", getpid());
            exit(1);
        } else if (pid == 0) { /* child */       
            for (int i = 0; i < loop_num; ++i) {
                printf("[pid %d] i = %d\n", getpid(), i);
            }
            _exit(0);
        } else { /* parent */
            pnum--;
            sleep(1);
        }
    }

    exit(0);
}
