/*
Project Group 7: Michael Nguyen, Ryan Latour, Maria Talhaferro Artur
Program: Lexer
Make a lexer using the flex generator
*/ 

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include "ast.h"
#include "parser_types.h"
#include "utilities.h"
#include "lexer.h"
#include "pl0_lexer.h"

extern int yydebug;

int main(int argc, char*argv[])
{
	// If no input file, bail with error
	if (argc < 2)
	{
		bail_with_error("No argument");
	}
	else
	{
		argv++;
		char *filename = argv[0];
		lexer_init(filename);
		lexer_output();
		return EXIT_SUCCESS;
	}
}