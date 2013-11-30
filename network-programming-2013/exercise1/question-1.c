#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>

void print_ps(void); /* print process IDs */
void print_env(); /* print env variables */

//get the execution time for certain func
void measure_time(void (*pfunc)(void)); 

// int main(int argc, char *argv[], char *envp[])
// {
//     while(*envp) {
//         printf("%s\n", *envp++);
//     }

//     return 0;
// }

int main(int argc, char *argv[])
{
    measure_time(print_ps);

    return 0;
}

void print_ps(void) {
    pid_t pid;
    pid_t ppid;
    uid_t uid;
    gid_t gid;

    pid = getpid();
    ppid = getppid();
    uid = getuid();
    gid = getgid();

    printf("pid: %d\n", pid);
    printf("ppid: %d\n", ppid);
    printf("uid: %d\n", uid);
    printf("gid: %d\n", gid);
}

void print_env(void) {
    // use extern char **environ in <unistd.h>
    while(*environ) {
        printf("%s\n", *environ++);
    }
}

void measure_time(void (*pfunc)(void)) {
    struct timeval start;
    struct timeval end;
    float sec, usec, diff;

    // gettimeofday from <sys/time.h>
    gettimeofday(&start, NULL);
    pfunc();
    gettimeofday(&end, NULL);

    sec = end.tv_sec - start.tv_sec;
    usec = end.tv_usec - start.tv_usec;
    diff = sec + usec / 1000000;
    printf("The executing time is %f\n", diff);
}