// Michael Nguyen
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "instruction.h"
#include "bof.h"

// Reminders
// Make sure to delete all print statements that aren't necessary

int main(int argc, char *arg[])
{
    if (argc < 2) // If there are no passed arguments
    {
        printf("No Passed Arguments\n");
        return 0;
    }

    BOFFILE bf; // Store the bof file somewhere

    if (strcmp(arg[1], "-p") == 0) // Uses -p option for tracing
    {
        printf("***Working On TRACING (.myo/.out)***\n");
        bf = bof_read_open(arg[2]); // Reading the bof file and storing a file pointer to bf
    }
    else // if no -p then print output
    {
        printf("***Working On OUTPUT (.myp/.lst)***\n");
        bf = bof_read_open(arg[1]); 
    }
    BOFHeader bfh = bof_read_header(bf);
    printf("Text Start Address = %u\n",bfh.text_start_address); // Is always 0?
    printf("Text Length = %u\n",bfh.text_length); // This will be the pc + 4, look at vm_test#.lst
    printf("Data Start Address = %u\n",bfh.data_start_address); 
    printf("Data Length = %u\n",bfh.data_start_address);
    printf("Stack Bottom Addr = %u\n",bfh.stack_bottom_addr); // Is always 4096?

    printf("\n*******************************************************************\n");



    bof_close(bf); // Done with bof file so close it
    return 0;
}

// To Compile
// gcc -o vm vm.c bof.c utilities.c instruction.c regname.c machine_types.c