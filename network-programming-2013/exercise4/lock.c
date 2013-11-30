#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define F_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

static struct flock lock_it, unlock_it;
static int lock_fd = -1;

void my_lock_init(char *path)
{
  struct stat st;
  int r;

  r = stat(path, &st);

  lock_fd = open(path, O_CREAT | O_WRONLY, F_MODE);

  if(lock_fd < 0) {
    perror("open lock file failed\n");
    exit(EXIT_FAILURE);
  }

  /* lock setup*/
  lock_it.l_type = F_WRLCK;
  lock_it.l_whence = SEEK_SET;
  lock_it.l_start = 0;
  lock_it.l_len = 0;

  /* unlock setup*/
  unlock_it.l_type = F_UNLCK;
  unlock_it.l_whence = SEEK_SET;
  unlock_it.l_start = 0;
  unlock_it.l_len = 0;
}

void my_lock_wait()
{
  int s;

  while((s = fcntl(lock_fd, F_SETLKW, &lock_it)) < 0) {
    if(errno == EINTR) {
      continue;
    }
    else {
      perror("fcntl() failed to release lock\n");
      exit(EXIT_FAILURE);
    }
  }
}

void my_lock_release()
{
  if(fcntl(lock_fd, F_SETLKW, &unlock_it) < 0) {
    perror("fcntl() failed to release lock\n");
    exit(EXIT_FAILURE);
  }
}