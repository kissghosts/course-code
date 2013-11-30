#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

#define F_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define SIZE 100000

void my_lock_init(char *path);
void my_lock_wait();
void my_lock_release();

int is_int(char const *s) 
{
  int n, i;
  return sscanf(s, "%d %n", &i, &n) == 1 && !s[n];
}

int main(int argc, char **argv)
{
  int fd, i, num_loop, zero = 0;
  char c, lock_f[128];
  struct stat st;
  int *iptr;
  char *cptr;

  if(argc != 4) {
    printf("usage: %s <pathname> <loop number> <char>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  if(is_int(argv[2]))
    num_loop = atoi(argv[2]);

  c = argv[3][0];

  i = stat(argv[1], &st); /* first one will get -1*/

  snprintf(lock_f, sizeof(lock_f), "%s.lock", argv[1]);

  if((fd = open(argv[1], O_RDWR | O_CREAT, F_MODE)) < 0) {
    perror("open() failed\n");
    exit(EXIT_FAILURE);
  }

  if(i == -1) { /* initialze */
    write(fd, &zero, sizeof(int)); /* int counter*/
    for(i = 0; i < SIZE; i++) {
      write(fd, &zero, 1);
    }
  }

  iptr = (int *)mmap(NULL, SIZE+sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

  if(iptr == MAP_FAILED) {
    perror("mmap failed\n");
    exit(EXIT_FAILURE);
  }

  close(fd); /* POSIX recommendation */

  cptr = (char *)(&iptr[1]);

  my_lock_init(lock_f);
  setbuf(stdout, NULL); /* stdout unbuffer */

  for(i = 0; i < num_loop; i++) {
    my_lock_wait();

    if(*iptr >= (SIZE-1)) {
      my_lock_release();
      exit(EXIT_SUCCESS);
    }

    //
    for ( ; ; ) {      
      if ((cptr[*iptr - 1]) == c && *iptr != 0) {
        my_lock_release();
        usleep(100);
        my_lock_wait();
      } else {
        break;
      }
    }

    cptr[*iptr] = c;
    printf("%c: %d\n", c, (*iptr)++);
    my_lock_release();
    // usleep(800000);
  }

  return 0;
}