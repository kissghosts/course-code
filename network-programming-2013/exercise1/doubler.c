#include <stdio.h>
#include <stdlib.h>

#define MAXLINE 4096
#define MAXLINES 4096

void double_print(FILE *fp);

int main(int argc, char *argv[])
{
    FILE *fp;

    if (argc == 1)
        double_print(stdin);
    else {
        if ((fp = fopen(*++argv, "r")) == NULL) {
            fprintf(stderr, "can't open %s\n", *argv);
            exit(1);
        } else 
            double_print(fp);
    }

    return 0;
}

void double_print(FILE *fp) {
    char buf[MAXLINE];

    while (fgets(buf, MAXLINE, fp) != NULL) {
        for (int i = 0; i < 2; ++i) {
            if (fputs(buf, stdout) == EOF) {
                fprintf(stderr, "can't write to stdout.\n");
                exit(1);
            }
        }
    }
}