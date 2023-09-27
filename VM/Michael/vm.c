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
        memory.sp[i] = 0;
        memory.gp[i] = data[i];
        length--;
        i++;
    }

    return data;
}

// This gets the index of the register passed
int regindex_get(char * input)
{
    for (int i = 0; i < 32; i++)
        if (strcmp(input, regname_get(i)) == 0) return i;
    return 0;
}

// Makes a basic Register and fills in necessary numbers
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

// The VM enforces the following invariants and will halt with an error message (written to stderr) if one of
// them is violated
bool checkInvariants(int *GPR, int i)
{
    int PC = i * BYTES_PER_WORD;

    // PC % BYTES_PER_WORD = 0
    if (PC % BYTES_PER_WORD != 0)
    {
        fprintf(stderr, "PC %% BYTES_PER_WORD != 0");
        newline(stderr);
        return true;
    }

    // GPR[$gp] % BYTES_PER_WORD = 0,
    if (GPR[regindex_get("$gp")] % BYTES_PER_WORD != 0)
    {
        fprintf(stderr, "GPR[$gp] %% BYTES_PER_WORD != 0");
        newline(stderr);
        return true;
    }

    // GPR[$sp] % BYTES_PER_WORD = 0
    if (GPR[regindex_get("$sp")] % BYTES_PER_WORD != 0)
    {
        fprintf(stderr, "GPR[$sp] %% BYTES_PER_WORD != 0");
        newline(stderr);
        return true;
    }

    // GPR[$fp] % BYTES_PER_WORD = 0
    if (GPR[regindex_get("$fp")] % BYTES_PER_WORD != 0)
    {
        fprintf(stderr, "GPR[$fp] %% BYTES_PER_WORD != 0");
        newline(stderr);
        return true;
    }

    // 0 ≤ GPR[$gp]
    if (!(0 <= GPR[regindex_get("$gp")]))
    {
        fprintf(stderr, "0 ≤ GPR[$gp]");
        newline(stderr);
        return true;
    }

    // GPR[$gp] < GPR[$sp]
    if (!(GPR[regindex_get("$gp")] < GPR[regindex_get("$sp")]))
    {
        fprintf(stderr, "GPR[$gp] < GPR[$sp]");
        newline(stderr);
        return true;
    }

    // GPR[$sp] ≤ GPR[$fp]
    if (!(GPR[regindex_get("$sp")] <= GPR[regindex_get("$fp")]))
    {
        fprintf(stderr, "GPR[$sp] ≤ GPR[$fp]");
        newline(stderr);
        return true;
    }

    // GPR[$fp] < MAX_STACK_HEIGHT

    // 0 ≤ PC
    if (!(0 <= PC))
    {
        fprintf(stderr, "PC >= 0");
        newline(stderr);
        return true;
    }

    // PC < MEMORY_SIZE_IN_BYTES
    if (!(PC < MEMORY_SIZE_IN_BYTES))
    {
        fprintf(stderr, "PC not less than MEMORY_SIZE_IN_BYTES");
        newline(stderr);
        return true;
    }

    // GPR[0] = 0
    if (GPR[0] != 0)
    {
        fprintf(stderr, "Wrong call to register 0");
        newline(stderr);
        return true;
    }

    return false;
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
    bool NOTR = false; // At the start we set NOTR to false until we find one
    char *instr = malloc(120 * sizeof(char));
    char *token[120]; 
    int length = (bh.text_length / BYTES_PER_WORD);

    int rs, rt, rd, immed, shift; // Indexes
    int LO, HI;
    LO = 0;
    HI = 0;

    for (int i = 0; i < length; i++)
    {
        strcpy(instr, instruct[i]);
        int index = 0;

        // STRA start tracing
        if (strcmp(instruct[i], "STRA") == 32 && i != 0) 
        {
            NOTR = false;
            continue;
        }
        // If NOTR is true we stop tracing until we find a STRA
        // But still do commands
        if (!NOTR)
        {
            // displays program counter and HI and LO if the aren't zero
            fprintf(out, "      PC: %d", i * BYTES_PER_WORD);
            if (HI != 0 || LO != 0)
                fprintf(out,"\tHI: %d\tLO: %d", HI, LO);
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

            fprintf(out, "==> addr: ");
            // displays assembly instruction
            printInstruct(out, i * BYTES_PER_WORD, instruct[i]);
            if (checkInvariants(GPR, i)) return;
        }

        // NOTR stop tracing
        if (strcmp(instruct[i], "NOTR") == 32)
            NOTR = true;

        // This is to split the instruction into separate parts to get the jump address
        token[index] = strtok(instr, " \t"); // splits if it sees a space or a tab(\t) or comma
        while (token[index] != NULL)
            token[++index] = strtok(NULL, " \t,");

        //ADD GPR[rd] <- GPR[rs] + GPR[rt]
        if (strcmp(token[0], "ADD") == 0)
        {
            rs = regindex_get(token[1]);
            rt = regindex_get(token[2]);
            rd = regindex_get(token[3]);

            GPR[rd] = GPR[rs] + GPR[rt];
        }
        // ADDI GPR[rt] <- GPR[rs] + Immediate
        else if (strcmp(token[0], "ADDI") == 0)
        {
            rs = regindex_get(token[1]);
            rt = regindex_get(token[2]);
            immed = atoi(token[3]);
            GPR[rt] = GPR[rs] + immed;
        }

        // SUB GPR[rd] <- GPR[rs] - GPR[rt]
        else if (strcmp(token[0], "SUB") == 0)
        {
            rs = regindex_get(token[1]);
            rt = regindex_get(token[2]);
            rd = regindex_get(token[3]);
            GPR[rd] = GPR[rs] - GPR[rt];
        }

        // AND GPR[rd] <- GPR[rs] & GPR[rt]
        else if (strcmp(token[0], "AND") == 0)
        {
            rs = regindex_get(token[1]);
            rt = regindex_get(token[2]);
            rd = regindex_get(token[3]);
            GPR[rd] = GPR[rs] & GPR[rt];
        }

        // ANDI GPR[rd] <- GPR[rs] & Immed
        else if (strcmp(token[0], "ANDI") == 0)
        {
            rs = regindex_get(token[1]);
            rt = regindex_get(token[2]);
            immed = atoi(token[3]);
            GPR[rd] = GPR[rs] & immed;
        }

        // OR/BOR GPR[rd] <- GPR[rs] | GPR[rt]
        else if (strcmp(token[0], "BOR") == 0)
        {
            rs = regindex_get(token[1]);
            rt = regindex_get(token[2]);
            rd = regindex_get(token[3]);
            GPR[rd] = GPR[rs] | GPR[rt];
        }

        // BORI GPR[rd] <- GPR[rs] | Immed
        else if (strcmp(token[0], "BORI") == 0)
        {
            rs = regindex_get(token[1]);
            rt = regindex_get(token[2]);
            immed = atoi(token[3]);
            GPR[rd] = GPR[rs] | immed;
        }

        // XOR GPR[rd] <- GPR[rs] ^ GPR[rt]
        else if (strcmp(token[0], "XOR") == 0)
        {
            rs = regindex_get(token[1]);
            rt = regindex_get(token[2]);
            rd = regindex_get(token[3]);
            GPR[rd] = GPR[rs] ^ GPR[rt];
        }

        // XORI GPR[rd] <- GPR[rs] ^ Immed
        else if (strcmp(token[0], "XORI") == 0)
        {
            rs = regindex_get(token[1]);
            rt = regindex_get(token[2]);
            immed = atoi(token[3]);
            GPR[rd] = GPR[rs] ^ immed;
        }

        //SLL GPR[rd] <- GPR[rs] << shift
        else if (strcmp(token[0], "SLL") == 0)
        {
            rt = regindex_get(token[1]);
            rd = regindex_get(token[2]);
            shift = atoi(token[3]);

            GPR[rd] = GPR[rt] << shift;
        }

        //SRL GPR[rd] <- GPR[rs] >> shift
        else if (strcmp(token[0], "SRL") == 0)
        {
            rt = regindex_get(token[1]);
            rd = regindex_get(token[2]);
            shift = atoi(token[3]);

            GPR[rd] = GPR[rt] >> shift;
        }

        // NOR GPR[d] ← ¬(GPR[s] ∨ GPR[t])
        else if (strcmp(token[0], "NOR") == 0)
        {
            rs = regindex_get(token[1]);
            rt = regindex_get(token[2]);
            rd = regindex_get(token[3]);
            GPR[rd] = ~(GPR[rs] | GPR[rt]);
        }

        // MUL (HI, LO) ← GPR[s] × GPR[t]
        else if (strcmp(token[0], "MUL") == 0)
        {
            rs = regindex_get(token[1]);
            rt = regindex_get(token[2]);
            int num = GPR[rs] * GPR[rt];
            LO = (num & 0x0000FFFF); // gets msb
            HI = (num & 0xFFFF0000); // gets lsb
        }

        // MFLO GPR[d] ← LO
        else if (strcmp(token[0], "MFLO") == 0)
        {
            rd = regindex_get(token[1]);
            GPR[rd] = LO;
        }

        // MFHI
        else if (strcmp(token[0], "MFHI") == 0)
        {
            rd = regindex_get(token[1]);
            GPR[rd] = HI;
        }

        // DIV HI ← GPR[s] % GPR[t]; LO ← GPR[s]/GPR[t])
        else if (strcmp(token[0], "DIV") == 0)
        {
            rs = regindex_get(token[1]);
            rt = regindex_get(token[2]);
            if (GPR[rs] == 0 || GPR[rd] == 0)
            {
                LO = 0;
                fprintf(stderr, "Division by 0");
            }
            else
            {
                LO = GPR[rs] / GPR[rt];
                HI = GPR[rs] % GPR[rt];
            }
        }

        // BEQ if GPR[s] = GPR[t] then PC ← PC + formOffset(o)
        else if (strcmp(token[0], "BEQ") == 0)
        {
            rs = regindex_get(token[1]);
            rt = regindex_get(token[2]);
            immed = atoi(token[3]);
            if (GPR[rs] == GPR[rt])
                i += machine_types_sgnExt(immed);
        }

        // BGEZ if GPR[s] ≥ 0 then PC ← PC + formOffset(o)
        else if (strcmp(token[0], "BGEZ") == 0)
        {
            rs = regindex_get(token[1]);
            immed = atoi(token[2]);
            if (GPR[rs] >= 0)
                i += machine_types_sgnExt(immed);
        }

        // BGTZ if GPR[s] ≥ 0 then PC ← PC + formOffset(o)
        else if (strcmp(token[0], "BGTZ") == 0)
        {
            rs = regindex_get(token[1]);
            immed = atoi(token[2]);
            if (GPR[rs] > 0)
                i += machine_types_sgnExt(immed);
        }

        // BLEZ if GPR[s] ≥ 0 then PC ← PC + formOffset(o)
        else if (strcmp(token[0], "BLEZ") == 0)
        {
            rs = regindex_get(token[1]);
            immed = atoi(token[2]);
            if (GPR[rs] <= 0)
                i += machine_types_sgnExt(immed);
        }

        // BLTZ if GPR[s] ≥ 0 then PC ← PC + formOffset(o)
        else if (strcmp(token[0], "BLTZ") == 0)
        {
            rs = regindex_get(token[1]);
            immed = atoi(token[2]);
            if (GPR[rs] < 0)
                i += machine_types_sgnExt(immed);
        }

        // BNE if GPR[s] ≥ 0 then PC ← PC + formOffset(o)
        else if (strcmp(token[0], "BNE") == 0)
        {
            rs = regindex_get(token[1]);
            rt = regindex_get(token[2]);
            immed = atoi(token[3]);
            if (GPR[rs] != GPR[rt])
                i += machine_types_sgnExt(immed);

        }

        // JMP PC <- a
        else if (strcmp(token[0], "JMP") == 0)
        {
            // Convert the string into an int
            int jmpNum = atoi(token[1]);
            i = jmpNum - 1; // Must sub one because i will be incremented in next iteration
        }

        // JAL PC <- a and GPR[$ra] = PC + 4
        else if (strcmp(token[0], "JAL") == 0)
        {
            GPR[regindex_get("$ra")] = i * BYTES_PER_WORD + BYTES_PER_WORD;
            i = (machine_types_formAddress(i, atoi(token[1])) / BYTES_PER_WORD) - 1;
        }

        // JR PC <- GPR[rs]
        else if (strcmp(token[0], "JR") == 0)
        {
            rs = regindex_get(token[1]);
            i = (GPR[rs] / BYTES_PER_WORD) - 1;
        }

        // PCH GPR[$v0] <- printf("%d", GPR[$a0]);
        // Prints out the letter version of whatever is in GPR[$a0]
        // We don't want a new line because the output only has one when PCH prints \n
        else if (strcmp(token[0], "PCH") == 0)
        {
            fprintf(out, "%c", GPR[regindex_get("$a0")]);
        }

        // IDK if this works or not
        // RCH GPR[$v0] <- getc(stdin)
        else if (strcmp(token[0], "RCH") == 0)
        {
            GPR[regindex_get("$v0")] = getc(stdin);
        }

        // LBU LW SB SW aren't functional but the general format is correct
        // NEED TO IMPLEMENT MEMORY BETTER

        // LW
        // The way i did the memory here is definitely wrong
        else if (strcmp(token[0], "LW") == 0)
        {
            rs = regindex_get(token[1]);;
            rt = regindex_get(token[2]);
            immed = atoi(token[3]);
            if (rs == regindex_get("$gp"))
                GPR[rt] = memory.gp[machine_types_sgnExt(immed)];
            else if (rs == regindex_get("$sp"))
                GPR[rt] = memory.sp[machine_types_sgnExt(immed)];
        }

        // LBU
        else if (strcmp(token[0], "LBU") == 0)
        {
            rs = regindex_get(token[1]);
            rt = regindex_get(token[2]);
            immed = atoi(token[3]);
            if (rs == regindex_get("$gp"))
                GPR[rt] = machine_types_zeroExt(memory.gp[machine_types_sgnExt(immed)]);
            else if (rs == regindex_get("$sp"))
                GPR[rt] = machine_types_zeroExt(memory.sp[machine_types_sgnExt(immed)]);
        }

        // SB
        // IDK how this is supposed to be done, but this is my attempt
        else if (strcmp(token[0], "SB") == 0)
        {
            rs = regindex_get(token[1]);
            rt = regindex_get(token[2]);
            immed = atoi(token[3]);
            if (rs == regindex_get("$gp"))
                memory.gp[machine_types_sgnExt(immed)] = GPR[rt] & 1;
            else if (rs == regindex_get("$sp"))
                memory.sp[machine_types_sgnExt(immed)] = GPR[rt] & 1;
        }

        // SW
        else if (strcmp(token[0], "SW") == 0)
        {
            rs = regindex_get(token[1]);
            rt = regindex_get(token[2]);
            immed = atoi(token[3]);
            if (rs == regindex_get("$gp"))
                memory.gp[machine_types_sgnExt(immed)] = GPR[rt];
            else if (rs == regindex_get("$sp"))
                memory.sp[machine_types_sgnExt(immed)] = GPR[rt];
        }

        // If there is an exit, we stop with no return
        else if (strcmp(instruct[i], "EXIT") == 32) return;
    }

    free(instr);
}

// Prints GPR table
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
        fprintf(out, "GPR[%-3s]: %d\t", regname_get(j), GPR[j]);
    }
    newline(out);
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
        if (data[i] == 0) break; 
        // line break when there are five on a line
        if (i % 5 == 0 && i != 0)
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
        bf = bof_read_open(arg[2]); // Reading the bof file and storing a file pointer to bf
        printOut(stdout, bf);
    }
    else // if no -p then print trace
    {
        bf = bof_read_open(arg[1]);
        trace(stdout, bf); 
    }
    
    bof_close(bf); // Done with bof file so close it
    return 0;
}

// To Compile
// gcc -o vm vm.c bof.c utilities.c instruction.c regname.c machine_types.c