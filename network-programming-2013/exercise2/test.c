#include <stdio.h>
#include <unistd.h>

// This is just a small program used for question 3

int main(int argc, char const *argv[])
{
    int i;

    for (i = 0; i < argc; i++)
        printf("argv[%d]: %s\n", i, argv[i]);
    
    return(0);
}
