// Michael Nguyen
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "instruction.h"
#include "bof.h"

int main(int argc, char *arg[])
{
    if (argc < 2) // If there are no passed arguments
    {
        printf("No Passed Arguments\n");
        return 0;
    }

    BOFFILE bf;

    if (strcmp(arg[1], "-p") == 0) // Uses -p option for tracing
    {
        printf("Working On OUTPUT (.myo/.out)\n\n");
        bf = bof_read_open(arg[2]); // Reading the bof file and storing it in bf
    }
    else // if no -p then print output
    {
        printf("Working On TRACING (.myp/.lst)\n\n");
        bf = bof_read_open(arg[1]); 
    }
    return 0;
}

// To Compile
// gcc -o vm vm.c bof.c utilities.c instruction.c regname.c machine_types.c