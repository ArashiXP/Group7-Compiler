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

// prints the instructions with three spaces
void printInstruct(FILE *out, bin_instr_t bi, unsigned int i)
{
    fprintf(out, "   %d %s", i, instruction_assembly_form(bi));
    newline(out);
}

void printData(FILE *out, BOFFILE bf, BOFHeader bh)
{
    fprintf(out, " %u:", bh.data_start_address);
    dataDec(out, bf, bh.data_length / BYTES_PER_WORD);
}

void dataDec(FILE *out, BOFFILE bf, int length)
{
    while (length > 0)
    {
        printDataDec(out, bof_read_word(bf));
        length--;
    }
}

void printDataDec(FILE *out, word_type w)
{
    if (w > 0)
        fprintf(out, "%d", w);
    else
        fprintf(out, "0");
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