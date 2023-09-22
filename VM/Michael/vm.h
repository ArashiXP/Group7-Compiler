#ifndef VM_H
#define VM_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "instruction.h"
#include "bof.h"

// For -p Tracing
void trace(BOFFILE bf);

// ****************************************************************

// ****************************************************************
// For Output
void printOut(FILE *out, BOFFILE bf);
void printText(FILE *out, BOFFILE bf, BOFHeader bh);
void printInstructHelper(FILE *out, BOFFILE bf, int length);
void printInstruct(FILE *out, bin_instr_t bi, unsigned int i);
void printData(FILE *out, BOFFILE bf, BOFHeader bh);

// ****************************************************************

#endif