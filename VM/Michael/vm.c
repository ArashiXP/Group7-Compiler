// Michael Nguyen
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "instruction.h"
#include "bof.h"
#include "utilities.h"
#include "regname.h"
#include "vm.h"

// Reminders
// Make sure to delete all print statements that aren't necessary
// Fix the formatting

// ***************************For Tracing**********************************
void trace(FILE *out, BOFFILE bf)
{
    printTracing(out, bf);
}

// Prints non '-p' command
void printTracing(FILE *out, BOFFILE bf)
{
    BOFHeader bh = bof_read_header(bf); // read the header
    bin_instr_t bi; // Hold the instruction
    bool NOTR = false; // At the start we set NOTR to false until we find one
    int length = (bh.text_length / BYTES_PER_WORD);
    for (int i = 0; i < length; i++)
    {
        bi = instruction_read(bf);
        // This is fufill the NOTR and STRA Command
        // if NOTR we stop until we get a STRA
        // if STRA isn't given we go until the program terminates
        if (strcmp(instruction_assembly_form(bi), "STRA") == 32 && i != 0) 
        {
            NOTR = false;
            continue;
        }
        // If NOTR is true we skip until we find a STRA
        if (NOTR) continue;

        // displays program counter
        fprintf(out, "      PC: %d", i * BYTES_PER_WORD);
        // displays General Purpose Register Table
        printGPR(out, bf, bh, i * BYTES_PER_WORD, bi);

        if (i != length)
        { 
            // So we don't display this for the final call
            // byte numbers at the end of GPR table
            // printData(out, bf, bh, bh.data_length / BYTES_PER_WORD);
            fprintf(out, "    %u: 0 ...", bh.stack_bottom_addr); // 4096
            newline(out);
        }

        // The instruction is NOTR so we set it to True
        if (strcmp(instruction_assembly_form(bi), "NOTR") == 32)
        {
            NOTR = true;
        }

        fprintf(out, "==> addr: ");
        // displays assembly instruction
        printInstruct(out, bi, i);
        
        // If there is an exit, we stop with no return
        if (strcmp(instruction_assembly_form(bi), "EXIT") == 32) return;
    }
}

// Prints GPR table and addr
void printGPR(FILE *out, BOFFILE bf, BOFHeader bh, unsigned int i, bin_instr_t bi)
{
    // GPR STUFF GOES HERE
    // $sp = stack pointer, $fp frame pointer, $gp data pointer
    for (int j = 0; j < NUM_REGISTERS; j++)
    {
        /*
        That is, the caller must save registers 1 − 15, and 24 − 25 if they will be needed after a call
        (and then restores them when needed).
        The callee saves (and restores before it returns) registers 16 − 23 and 29 − 31, if it uses 
        (writes) them. (Furthermore, register 0 cannot be changed and registers 1 and 28 should
        not be changed by a hand-written routine. Registers 26 − 27 should not be changed by user code.)
        */

        // cleans output to add a new line after every 6 fprints
        if (j % 6 == 0)
            newline(out);

        // regname function belongs to regname.h

        // assigns GPR[$fp] (register 30)
        if (strcmp("$fp", regname_get(j)) == 0)
            fprintf(out, "GPR[%s]: %u\t", regname_get(j), bh.stack_bottom_addr);
        // assigns GPR[$gp] (register 28)
        else if (strcmp("$gp", regname_get(j)) == 0)
            fprintf(out, "GPR[%s]: %u\t", regname_get(j), bh.data_start_address);
        // assigns GPR[$sp] 
        else if (strcmp("$sp", regname_get(j)) == 0)
            fprintf(out, "GPR[%s]: %u\t", regname_get(j), bh.stack_bottom_addr);
        else
            fprintf(out, "GPR[%-3s]: 0\t", regname_get(j)); // "base" case
    }
    newline(out);
}
// *************************************************************************

// ***************************For Output************************************

// Take in the header file and print out the correct format
void printOut(FILE *out, BOFFILE bf)
{
    BOFHeader bh = bof_read_header(bf);
    printText(out, bf, bh);
    printData(out, bf, bh, bh.data_length / BYTES_PER_WORD);
}

// Print the title and first half of the output
void printText(FILE *out, BOFFILE bf, BOFHeader bh)
{
    fprintf(out, "Addr Instruction");
    newline(out);
    int length = (bh.text_length / BYTES_PER_WORD);
    // Will call a function to print the PC and instruction
    for (int i = 0; i < length; i++)
        printInstruct(out, instruction_read(bf), i * BYTES_PER_WORD);
}

// prints the PC and instructions with two spaces
void printInstruct(FILE *out, bin_instr_t bi, unsigned int i)
{
    fprintf(out, "  %3d %s", i, instruction_assembly_form(bi));
    newline(out);
}

// This will print the bottom stuff
void printData(FILE *out, BOFFILE bf, BOFHeader bh, int length)
{  
    unsigned int num = bh.data_start_address;
    int i = 0; // To help with indenting
    while (length > 0)
    {
        if (i % 5 == 0)
            newline(out);
        fprintf(out, "   %u: %d\t", num, bof_read_word(bf));
        num += 4; // Always increment by four
        length--;
        i++;
    }
    // reached the end, print default
    fprintf(out,"   %u: 0 ...", num); // 1024 ...
    newline(out);
}

// *************************************************************************

int main(int argc, char *arg[])
{
    if (argc < 2) // If there are no passed arguments
        return 0;

    BOFFILE bf; // Store the bof file somewhere

    if (strcmp(arg[1], "-p") == 0) // Uses -p option for output
    {
        printf("***Working On OUTPUT (.myp/.lst)***\n"); // TO BE REMOVED
        bf = bof_read_open(arg[2]); // Reading the bof file and storing a file pointer to bf
        printOut(stdout, bf);
    }
    else // if no -p then print trace
    {
        printf("***Working On TRACING (.myo/.out)***\n"); // TO BE REMOVED
        bf = bof_read_open(arg[1]); 
        trace(stdout, bf); 
    }
    
    bof_close(bf); // Done with bof file so close it
    return 0;
}

// To Compile
// gcc -o vm vm.c bof.c utilities.c instruction.c regname.c machine_types.c