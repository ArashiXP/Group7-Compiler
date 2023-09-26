// Michael Nguyen
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "instruction.h"
#include "bof.h"
#include "utilities.h"
#include "regname.h"
#include "vm.h"

#define MEMORY_SIZE_IN_BYTES (65536 - BYTES_PER_WORD)

// Reminders
// Make sure to delete all print statements that aren't necessary
// Fix the formatting

// ***************************HELPER FUNCTIONS*****************************
// This is to store all of the instructions so we can access them easier
char** instructionList (BOFHeader bh, BOFFILE bf)
{
    int length = bh.text_length / BYTES_PER_WORD;
    bin_instr_t bi;

    // Make the 2d array to hold all instructions
    char** instructions = malloc(length * sizeof(char*));
    for (int i = 0; i < length; i++)
        instructions[i] = malloc(60 * sizeof(char));

    for (int i = 0; i < length; i++)
    {
        bi = instruction_read(bf);
        strcpy(instructions[i], instruction_assembly_form(bi));
    }
    return instructions;
}

// This function is to put all of the numbers at the bottom into an array to access
int* dataList(BOFFILE bf, BOFHeader bh)
{
    int length = bh.data_length / BYTES_PER_WORD;
    int* data = malloc(length * sizeof(int));
    int i = 0;
    while (length > 0)
    {
        data[i] = bof_read_word(bf);
        length--;
        i++;
    }

    return data;
}

int regindex_get(char * input)
{
    for (int i = 0; i < 32; i++)
        if (strcmp(input, regname_get(i)) == 0) return i;
    return 0;
}

int* makeRegister(BOFHeader bh)
{
    int* registers = malloc(NUM_REGISTERS * sizeof(int));
    for (int i = 0; i < NUM_REGISTERS; i++)
    {
        if (i == 28)
            registers[i] = bh.data_start_address;
        else if (i == 29)
            registers[i] = bh.stack_bottom_addr;
        else if (i == 30)
            registers[i] = bh.stack_bottom_addr;
        else
            registers[i] = 0;
    }

    return registers;
}

// *************************************************************************

// ***************************For Tracing***********************************
void trace(FILE *out, BOFFILE bf)
{
    BOFHeader bh = bof_read_header(bf); // read the header
    char **instr = instructionList(bh, bf); // Array to hold instructions
    int *data = dataList(bf, bh); //Array to hold the numbers at the bottom 1024: 33 ...
    int* GPR = makeRegister(bh); // Array to hold the registers

    printTracing(out, bf, bh, instr, data, GPR);

    // Free everything
    for (int i = 0; i < bh.text_length / BYTES_PER_WORD; i++)
        free(instr[i]);
    free(instr);
    free(data);
    free(GPR);
}

// Prints non '-p' command
void printTracing(FILE *out, BOFFILE bf, BOFHeader bh, char ** instruct, int* data, int* GPR)
{
    bin_instr_t bi; // Hold the instruction
    BOFFILE temp = bf; // Keep a copy of the bf, so we don't iterate it
    bool NOTR = false; // At the start we set NOTR to false until we find one
    char *instr = malloc(120 * sizeof(char));
    char *token[120]; 
    int length = (bh.text_length / BYTES_PER_WORD);

    int rs, rt, rd, immed; // Indexes

    for (int i = 0; i < length; i++)
    {
        strcpy(instr, instruct[i]);
        int index = 0;

        // This is fufill the NOTR and STRA Command
        // if NOTR we stop until we get a STRA
        // if STRA isn't given we go until the program terminates
        if (strcmp(instruct[i], "STRA") == 32 && i != 0) 
        {
            NOTR = false;
            continue;
        }
        // If NOTR is true we skip until we find a STRA
        if (NOTR) continue;

        // displays program counter
        fprintf(out, "      PC: %d", i * BYTES_PER_WORD);
        // displays General Purpose Register Table
        printGPR(out, GPR);

        if (i != length)
        { 
            // So we don't display this for the final call
            // byte numbers at the end of GPR table
            printData(out, bh, bh.data_length / BYTES_PER_WORD, data);
            fprintf(out, "    %u: 0 ...", bh.stack_bottom_addr); // 4096
            newline(out);
        }

        // The instruction is NOTR so we set it to True
        if (strcmp(instruct[i], "NOTR") == 32)
            NOTR = true;

        fprintf(out, "==> addr: ");
        // displays assembly instruction
        printInstruct(out, i * BYTES_PER_WORD, instruct[i]);

        // This is to split the instruction into separate parts to get the jump address
        token[index] = strtok(instr, " \t"); // splits if it sees a space or a tab(\t) or comma
        while (token[index] != NULL)
            token[++index] = strtok(NULL, " \t,");

        // ADDI GPR[rt] <- GPR[rs] + Immediate
        if (strcmp(token[0], "ADDI") == 0)
        {
            rs = regindex_get(token[1]);
            rt = regindex_get(token[2]);
            immed = atoi(token[3]);
            GPR[rt] = GPR[rs] + immed;
        }

        //ADD GPR[rd] <- GPR[rs] + GPR[rt]
        if (strcmp(token[0], "ADD") == 0) {
            rs = regindex_get(token[1]);
            rt = regindex_get(token[2]);
            rd = regindex_get(token[3]);

            GPR[rd] = GPR[rs] + GPR[rt];

        }

        // SUB GPR[rd] <- GPR[rs] - GPR[rt]
        if (strcmp(token[0], "SUB") == 0) {
            rs = regindex_get(token[1]);
            rt = regindex_get(token[2]);
            rd = regindex_get(token[3]);
            GPR[rd] = GPR[rs] - GPR[rt];
        }

        // Jump function
        // if the command is JMP or JAL we take the second token (the number) and jump to that
        if (strcmp(token[0], "JMP") == 0 || strcmp(token[0], "JAL") == 0)
        {
            // Convert the string into an int
            int jmpNum = atoi(token[1]);
            i = jmpNum - 1; // Must sub one because i will be incremented in next iteration
        }
        
        // If there is an exit, we stop with no return
        if (strcmp(instruct[i], "EXIT") == 32) return;
    }

    free(instr);
}

// Prints GPR table and addr
void printGPR(FILE *out, int* GPR)
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

        fprintf(out, "GPR[%-3s]: %d\t", regname_get(j), GPR[j]);
    }
}
// *************************************************************************

// ***************************For Output************************************

// Take in the header file and print out the correct format
void printOut(FILE *out, BOFFILE bf)
{
    BOFHeader bh = bof_read_header(bf);
    char **instructions = instructionList(bh, bf); // hold instructions
    int *data = dataList(bf, bh); // hold bottom addresses
    printText(out, bf, bh, instructions);
    printData(out, bh, bh.data_length / BYTES_PER_WORD, data);
    
    // Free everything
    for (int i = 0; i < bh.text_length / BYTES_PER_WORD; i++)
        free(instructions[i]);
    free(instructions);
    free(data);
}

// Print the title and first half of the output
void printText(FILE *out, BOFFILE bf, BOFHeader bh, char ** instr)
{
    fprintf(out, "%-4s %s\n", "Addr", "Instruction");
    int length = (bh.text_length / BYTES_PER_WORD);
    // Will call a function to print the PC and instruction
    for (int i = 0; i < length; i++)
        printInstruct(out, i * BYTES_PER_WORD, instr[i]);
}

// prints the PC and instructions with two spaces
void printInstruct(FILE *out, unsigned int i, char * instr)
{
    fprintf(out, "  %3d %s", i, instr);
    newline(out);
}

// This will print the bottom stuff
void printData(FILE *out, BOFHeader bh, int length, int* data)
{  
    unsigned int num = bh.data_start_address;
    int i = 0; // To help with indenting
    while (length > 0)
    {
        // line break when there are five on a line
        if (i % 5 == 0)
            newline(out);

        fprintf(out, "    %u: %d\t", num, data[i]);
        num += 4; // Always increment by four
        length--;
        i++;
    }
    // reached the end, print default
    fprintf(out,"    %u: 0 ...", num); // 1024 ...
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