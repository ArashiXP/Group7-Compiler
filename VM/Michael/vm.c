// Michael Nguyen
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "instruction.h"
#include "bof.h"
#include "utilities.h"
#include "vm.h"

// Reminders
// Make sure to delete all print statements that aren't necessary

// ***************************For Tracing**********************************
void trace(BOFFILE bf)
{

}
// *************************************************************************

// ***************************For Output************************************

void printOut(FILE *out, BOFFILE bf)
{
    BOFHeader bh = bof_read_header(bf);
    printText(out, bf, bh);
    printData(out, bf, bh);
}

// Print the title and first half of the output
void printText(FILE *out, BOFFILE bf, BOFHeader bh)
{
    fprintf(out, "Addr Instruction");
    newline(out);
    int length = (bh.text_length / BYTES_PER_WORD);
    for (int i = 0; i < length; i++)
    {
        
        printInstruct(out, instruction_read(bf), i * BYTES_PER_WORD);
    }
}

// prints the instructions with two spaces
void printInstruct(FILE *out, bin_instr_t bi, unsigned int i)
{
    fprintf(out, "  %3d %s", i, instruction_assembly_form(bi));
    newline(out);
}

void printData(FILE *out, BOFFILE bf, BOFHeader bh)
{
    int length = bh.data_length / BYTES_PER_WORD;
    unsigned int num = bh.data_start_address;
    while (length > 0)
    {
        fprintf(out, "   %u: %d\t", num, bof_read_word(bf));
        num += 4;
        length--;
    }
    fprintf(out, "%u: 0 ...", num);
    newline(out);
}

// *************************************************************************

int main(int argc, char *arg[])
{
    if (argc < 2) // If there are no passed arguments
        return 0;

    BOFFILE bf; // Store the bof file somewhere

    if (strcmp(arg[1], "-p") == 0) // Uses -p option for tracing
    {
        printf("***Working On TRACING (.myo/.out)***\n");
        bf = bof_read_open(arg[2]); // Reading the bof file and storing a file pointer to bf
        trace(bf); 
    }
    else // if no -p then print output
    {
        printf("***Working On OUTPUT (.myp/.lst)***\n");
        bf = bof_read_open(arg[1]); 
        printOut(stdout, bf);
    }

    bof_close(bf); // Done with bof file so close it
    return 0;
}

// To Compile
// gcc -o vm vm.c bof.c utilities.c instruction.c regname.c machine_types.c