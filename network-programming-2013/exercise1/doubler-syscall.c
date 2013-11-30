#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFFSIZE 4096
#define MAXLINES 4096

void double_print(int file); /* write the input twice to stdout*/

// use system calls read, write and open

int main(int argc, char *argv[])
{
    int file; /* file descriptor */

    if (argc == 1)
        double_print(STDIN_FILENO); /* defined in <unistd.h> */
    else {
        if ((file = open(*++argv, O_RDONLY)) == -1) {
            // still use lib func to write err info
            fprintf(stderr, "can't open %s\n", *argv);
            exit(1);
        } else 
            double_print(file);
    }

    return 0;
}

void double_print(int file) {
    int n;
    char buf[BUFFSIZE];

    while ((n = read(file, buf, BUFFSIZE)) > 0) {
        for (int i = 0; i < 2; i++) {
            if (write(STDOUT_FILENO, buf, n) != n) {
                fprintf(stderr, "can't write to stdout\n");
                exit(1);
            }
        }
    }
}