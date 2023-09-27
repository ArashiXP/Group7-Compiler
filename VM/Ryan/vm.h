#ifndef VM_H
#define VM_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "instruction.h"
#include "bof.h"

#define MEMORY_SIZE_IN_BYTES (65536 - BYTES_PER_WORD)

union Memory
{
    int gp[MEMORY_SIZE_IN_BYTES];
    int sp[MEMORY_SIZE_IN_BYTES];
}memory;

char** instructionList (BOFHeader bh, BOFFILE bf);
int* dataList(BOFFILE bf, BOFHeader bh);
int* makeRegister(BOFHeader bh);
int regindex_get(char * input);

// For Tracing
void trace(FILE *out, BOFFILE bf);
void printTracing(FILE *out, BOFFILE bf, BOFHeader bh, char ** instruct, int* data, int* GPR);
void printGPR(FILE *out, int* GPR);

// ****************************************************************

// ****************************************************************
// For -p Output
void printOut(FILE *out, BOFFILE bf);
void printText(FILE *out, BOFFILE bf, BOFHeader bh, char ** instr);
void printInstruct(FILE *out, unsigned int i, char * instr);
void printData(FILE *out, BOFHeader bh, int length, int* data);

// ****************************************************************

#endif