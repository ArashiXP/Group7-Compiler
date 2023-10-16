#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include "ast.h"
#include "parser_types.h"
#include "utilities.h"
#include "lexer.h"
#include "pl0_lexer.h"

int main(int argc, char*argv[])
{
	argc--;
	argv++;
	char *filename = argv[0];
	lexer_init(filename);
	lexer_output();
	return 0;
}