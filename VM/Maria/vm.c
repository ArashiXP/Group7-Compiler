#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "instruction.h"
#include "bof.h"
#include "utilities.h"
#include "vm.h"
#include "regname.h"

// Reminders
// Make sure to delete all print statements that aren't necessary
// Fix the formatting

// ***************************For Tracing**********************************
void trace(FILE *out, BOFFILE bf)
{
    printTracing(out, bf);
}

// Prints '-p' command
void printTracing(FILE *out, BOFFILE bf)
{
    int i = 0;
    BOFHeader bh = bof_read_header(bf);
    int length = (bh.text_length / BYTES_PER_WORD);
    for (i = 0; i < length; i++)
    {
        // displays program counter
        fprintf(out, "  PC = %d", i * BYTES_PER_WORD);
        newline(out);
        // displays General Purpose Register Table
        GPR(out, bf, bh, i);

        // byte numbers at the end of GPR table
        fprintf(out, "  %u: ", bh.data_start_address); // 1024
        newline(out);
        fprintf(out, "  %u:", bh.stack_bottom_addr); // 4096
        newline(out);
    }
}

// Prints GPR table and addr
void printGPR(FILE *out, BOFFILE bf, BOFHeader bh, unsigned int i)
{
    // need to get the string containting the instructions in assempbly form
    bin_instr_t bi = instruction_read(bf); // reads instruction in binary form
    int length = strlen(instruction_assembly_form(bi));
    char *instr = malloc(sizeof(char) * (length + 1));
    // copying the instruction since the function returns it in constant form
    instr = strcpy(instr, instruction_assembly_form(bi));

    // GPR STUFF GOES HERE
    // $sp = stack pointer, $fp frame pointer, $gp data pointer
    for (int j = 0; j < NUM_REGISTERS; j++)
    {
        // EX: addr: ADDI $0, $t0, 1
        // I tried to split this string and then if we arrive at the GPR[$t0],
        // I would add the 1 to the register changing it to GPR[$t0]: 1
        // as seen in the vm_test0.out

        /*if (strstr(instruction_assembly_form(bi), regname_get(j)))
        {
            char *token = strtok(instr, "$");
            while (token != NULL)
            {
                fprintf(out, "++++++TOKEN: %s++++++", token);
                token = strtok(NULL, "$");
                if (strcmp(token, regname_get(j)))
                {
                    token = strtok(NULL, "$");
                    fprintf(out, "GPR[%s]: %s   ", regname_get(j), token);
                    break;
                }
            }
        }
        */

        // cleans output to add a new line after every 6 fprints
        if (j % 6 == 0)
            newline(out);

        // regname function belongd to regname.h

        // assigns GPR[$fp]
        if (strcmp("$fp", regname_get(j)) == 0)
            fprintf(out, "GPR[%s]: %u   ", regname_get(j), bh.stack_bottom_addr);
        // assigns GPR[$gp]
        else if (strcmp("$gp", regname_get(j)) == 0)
            fprintf(out, "GPR[%s]: %u   ", regname_get(j), bh.data_start_address);
        else
            fprintf(out, "GPR[%s]: 0   ", regname_get(j)); // "base" case
    }
    newline(out);
    fprintf(out, "==> addr: ");
    // displays assembly instruction
    printInstruct(out, bi, i);
}
// *************************************************************************

// ***************************For Output************************************

// Take in the header file and print out the correct format
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
void printData(FILE *out, BOFFILE bf, BOFHeader bh)
{
    int length = bh.data_length / BYTES_PER_WORD;
    unsigned int num = bh.data_start_address;
    while (length > 0)
    {
        fprintf(out, "   %u: %d\t", num, bof_read_word(bf));
        num += 4; // Always increment by four
        length--;
    }
    // reached the end, print default
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
        printf("***Working On TRACING (.myo/.out)***\n"); // TO BE REMOVED
        bf = bof_read_open(arg[2]);                       // Reading the bof file and storing a file pointer to bf
        trace(stdout, bf);
    }
    else // if no -p then print output
    {
        printf("***Working On OUTPUT (.myp/.lst)***\n"); // TO BE REMOVED
        bf = bof_read_open(arg[1]);
        printOut(stdout, bf);
    }

    bof_close(bf); // Done with bof file so close it
    return 0;
}

// gcc -o vm vm.c bof.c utilities.c instruction.c regname.c machine_types.c