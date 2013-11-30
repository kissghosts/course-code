#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/* default permissions for new files */
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define MAXSIZE 30

static struct flock lock_it, unlock_it;
static int lock_fd = -1; /* fcntl() will fail if my_lock_init() not called */

void my_lock_init(int fd);
void my_lock_wait();
void my_lock_release();

int main(int argc, char **argv)
{
    int fd, i, j, nloop, zero = 0;
    char input_char;
    char x[MAXSIZE];
    int *ptr;
    char *pchar;
    pid_t pid;
    struct stat buf;

    if (argc != 4) {
        perror("usage: incr2 <pathname> <nloop> <char>"); 
        exit(1); 
    }
    nloop = atoi(argv[2]);
    input_char = argv[3][0];

    /* open file, initialize to 0, map into memory */
    // stat() in <sys/stat.h>, check whether this is the first to open the file
    if (stat(argv[1], &buf) == -1 && errno == ENOENT) {
        /* first time, using O_CREAT */
        if ((fd = open(argv[1], O_RDWR | O_CREAT, FILE_MODE)) < 0) {
            perror("create failed"); 
            exit(1);
        }
        if (write(fd, &zero, sizeof(int)) <0) {
            perror("write failed"); 
            exit(1);
        }
    } else if ((fd = open(argv[1], O_RDWR, FILE_MODE)) < 0) {
        perror("open failed"); 
        exit(1);
    }
    
    ptr = (int *)mmap(NULL, sizeof(int) + sizeof(x), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap failed"); 
        exit(1); 
    }

    pchar = (char *)ptr;

    /* create, initialize, and unlink file lock */
    my_lock_init(fd);

    setbuf(stdout, NULL);   /* stdout is unbuffered */
    
    for (i = 0; i < nloop; i++) {
        my_lock_wait();
        printf("pid %d: count=%d\n", getpid(), ++(*ptr));
        printf("%d\n", *ptr);

        *(pchar + *ptr) = input_char;
        for (j = 1; j <= *ptr; j++) {
            putchar(*(pchar + j));
        }
        // fflush();
        
        my_lock_release();
        sleep(3);
    }

    my_lock_wait();
    printf("pid %d: get lock\n", getpid());
    sleep(5);
    printf("pid %d: %d\n", getpid(), (*ptr)++);
    my_lock_release();

    close(fd);
    unlink(argv[1]);
    exit(0);
}

void my_lock_init(int fd)
{
    lock_fd = fd;
    if (lock_fd < 0) {
        perror("lock file open failed"); 
        exit(1); 
    }
    
    lock_it.l_type = F_WRLCK;
    lock_it.l_whence = SEEK_SET;
    lock_it.l_start = 0;
    lock_it.l_len = 0;

    unlock_it.l_type = F_UNLCK;
    unlock_it.l_whence = SEEK_SET;
    unlock_it.l_start = 0;
    unlock_it.l_len = 0;
}

void my_lock_wait()
{
    int rc;
    
    while ((rc = fcntl(lock_fd, F_SETLKW, &lock_it)) < 0) {
        if (errno == EINTR)
            continue;
        else {
            perror("fcntl error for my_lock_wait"); 
            exit(1); 
        }
    }
}

void my_lock_release()
{
    if (fcntl(lock_fd, F_SETLKW, &unlock_it) < 0) {
        perror("fcntl error for my_lock_release"); 
        exit(1); 
    }
}

void print_char_table(int count)
{

}
