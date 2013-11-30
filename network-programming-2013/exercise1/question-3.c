#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

void print_stat(int file); /* print file attributes */

int main(int argc, char *argv[])
{
    int file; /* file descriptor */

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    else {
        if ((file = open(*++argv, O_RDONLY)) == -1) {
            // still use lib func to write err info
            fprintf(stderr, "can't open %s\n", *argv);
            exit(EXIT_FAILURE);
        } else 
            print_stat(file);
    }

    exit(EXIT_SUCCESS);
}

void print_stat(int file) {
    struct stat buf;

    // fstat() in <sys/stat.h>
    if (fstat(file, &buf) == -1) {
        fprintf(stderr, "fail to get stat info\n");
        exit(EXIT_FAILURE);
    }

    if (S_ISREG(buf.st_mode))
        printf("Type: regular file\n");
    else if (S_ISDIR(buf.st_mode))
        printf("Type: directory\n");
    else if (S_ISCHR(buf.st_mode))
        printf("Type: character device\n");
    else if (S_ISBLK(buf.st_mode))
        printf("Type: block device\n");
    else if (S_ISFIFO(buf.st_mode))
        printf("Type: fifo/pipe\n");
    else if (S_ISSOCK(buf.st_mode))
        printf("Type: socket\n");
    else
        printf("Type: unknown type\n");

    printf("Ownership: UID=%ld, GID=%ld\n", 
        (long) buf.st_uid, (long) buf.st_gid);
    printf("File size: %lld bytes\n", (long long) buf.st_size);

}