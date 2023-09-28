// COP3402
// Project Group 7: 
// Michael Nguyen, Ryan Latour, Maria Talhaferro Artur
// **************
// Program: VM.c
// **************
// Implement a Simplified RISC Machine (SRM) which is a virtual machine based on the MIPS processor
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "instruction.h"
#include "bof.h"
#include "utilities.h"
#include "regname.h"
#include "vm.h"

// This is to store all of the instructions
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

// Stores into memory
void buildGlobal(BOFFILE bf, BOFHeader bh, int * GPR) {
    int length = bh.data_length / BYTES_PER_WORD;
    
    for (int i = GPR[GP]; i < GPR[GP] + (length * 4); i = i + 4) 
        memory.sp[i] = bof_read_word(bf);
}

// Gets the index of the register passed
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

// The VM enforces the following invariants and will halt with an error message 
// (written to stderr) if one of them is violated
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

void trace(FILE *out, BOFFILE bf)
{
    BOFHeader bh = bof_read_header(bf); // read the header
    char **instr = instructionList(bh, bf); // Array to hold instructions
    int* GPR = makeRegister(bh); // Array to hold the registers
    buildGlobal(bf, bh, GPR);
    printTracing(out, bf, bh, instr, GPR);
    // Free everything
    for (int i = 0; i < bh.text_length / BYTES_PER_WORD; i++)
        free(instr[i]);
    free(instr);
    free(GPR);
}

// Prints non '-p' command
void printTracing(FILE *out, BOFFILE bf, BOFHeader bh, char ** instruct, int* GPR)
{
    bool NOTR = false; // At the start we set NOTR to false until we find one
    char *instr = malloc(120 * sizeof(char));
    char *token[120]; 
    int length = (bh.text_length / BYTES_PER_WORD);
    int rs, rt, rd, immed, shift, LO, HI;
    LO = HI = 0;
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
                printData(out, bh, bh.data_length / BYTES_PER_WORD, GPR);
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
        // This is to split the instruction into separate parts
        token[index] = strtok(instr, " \t");
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
            LO = (num & 0x000000FF);
            HI = (num & 0xFFFFFF00);
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
            // Must sub one because i will be incremented in next iteration
            i = jmpNum - 1; 
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
        else if (strcmp(token[0], "PCH") == 0)
        {
            fprintf(out, "%c", GPR[regindex_get("$a0")]);
            GPR[regindex_get("$v0")] = GPR[regindex_get("$a0")];
        }

        // RCH GPR[$v0] <- getc(stdin)
        else if (strcmp(token[0], "RCH") == 0)
        {
            GPR[regindex_get("$v0")] = getc(stdin);
        }


        // LW
        else if (strcmp(token[0], "LW") == 0)
        {
            rs = regindex_get(token[1]);
            rt = regindex_get(token[2]);
            immed = atoi(token[3]);
            GPR[rt] = memory.sp[GPR[rs] + machine_types_sgnExt(immed * 4)];
        }

        // LBU
        else if (strcmp(token[0], "LBU") == 0)
        {
            rs = regindex_get(token[1]);
            rt = regindex_get(token[2]);
            immed = atoi(token[3]);
            GPR[rt] = machine_types_zeroExt(memory.sp[GPR[rs] + machine_types_sgnExt(immed * 4)]);
            
        }

        // SB
        else if (strcmp(token[0], "SB") == 0)
        {
            rs = regindex_get(token[1]);
            rt = regindex_get(token[2]);
            immed = atoi(token[3]);
            if (rs == regindex_get("$gp"))
                memory.gp[GPR[GP] + machine_types_sgnExt(immed * 4)] = GPR[rt] & 1;
            else if (rs == regindex_get("$sp"))
                memory.sp[GPR[SP] + machine_types_sgnExt(immed * 4)] = GPR[rt] & 1;
        }

        // SW
        else if (strcmp(token[0], "SW") == 0)
        {   
            rs = regindex_get(token[1]);
            rt = regindex_get(token[2]);
            immed = atoi(token[3]);
            memory.sp[GPR[rs] + machine_types_sgnExt(immed * 4)] = GPR[rt];

        }
        // If there is an exit, we stop with no return
        else if (strcmp(instruct[i], "EXIT") == 32) return;
    }
    free(instr);
}

// Prints GPR table
void printGPR(FILE *out, int* GPR)
{
    for (int j = 0; j < NUM_REGISTERS; j++)
    {
        if (j % 6 == 0)
            newline(out);
        fprintf(out, "GPR[%-3s]: %d\t", regname_get(j), GPR[j]);
    }
    newline(out);
}

// Take in the header file and print out the correct format
void printOut(FILE *out, BOFFILE bf)
{
    BOFHeader bh = bof_read_header(bf);
    char **instructions = instructionList(bh, bf); // hold instructions
    printText(out, bf, bh, instructions);
    int length = bh.data_length / BYTES_PER_WORD;
    int *GPR = makeRegister(bh);
    buildGlobal(bf, bh, GPR);
    
    // Prints memory
    int line  = 0;
    for (int i = GPR[GP]; i <= GPR[GP] + (4 * length); i = i + 4) 
    {
      if (line % 5 == 0 && line > 0)
            newline(out);
      if (memory.sp[i] == 0 && memory.sp[i - 4] == 0 && i != GPR[GP]) continue;  
      fprintf(out, "    %d: %d	", i, memory.sp[i]);
      if (memory.sp[i] == 0) fprintf(out, " ...");
      line++;
    }
    newline(out);
    // Free everything
    for (int i = 0; i < bh.text_length / BYTES_PER_WORD; i++)
        free(instructions[i]);
    free(instructions);
    free(GPR);
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

// This function prints memory
void printData(FILE *out, BOFHeader bh, int length, int * GPR)
{
    int line  = 0;
    for (int i = GPR[GP]; i <= GPR[GP] + (4 * length); i = i + 4) {
      if (line % 5 == 0 && line > 0) newline(out);      
      if (memory.sp[i] == 0 && memory.sp[i - 4] == 0 && i != GPR[GP]) continue;    
      fprintf(out, "    %d: %d	", i, memory.sp[i]);
      if (memory.sp[i] == 0) fprintf(out, " ...");
      line++;
    }
    newline(out);
    line = 0;
    // Iterate from stack pointer to frame pointer
    for (int i = GPR[SP]; i <= GPR[FP]; i = i + 4) 
    {
        if (line % 5 == 0 && line > 0) newline(out); 
        if (memory.sp[i] == 0 && memory.sp[i - 4] == 0 && (i-4) >= GPR[SP]) continue;
        fprintf(out, "    %d: %d	", i, memory.sp[i]); 
        if (memory.sp[i] == 0) fprintf(out, " ...");
        line++;
    }
    newline(out);
}

int main(int argc, char *arg[])
{
    // If there are no passed arguments
    if (argc < 2) return 0;
    BOFFILE bf;
    // Uses -p option for output
    if (strcmp(arg[1], "-p") == 0) 
    {
        bf = bof_read_open(arg[2]);
        printOut(stdout, bf);
    }
    // if no -p then print trace
    else
    {
        bf = bof_read_open(arg[1]);
        trace(stdout, bf); 
    }
    bof_close(bf);
    return 0;
}