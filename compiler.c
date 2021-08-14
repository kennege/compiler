#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "token.h"
#include "lexer.h"
#include "parser.h"
#include "utils.h"

int main() 
{
    /* TODO: read file, lex file, parse tokens to AST, convert AST to machine code, 
    save machine code to file */
    
    char *input_str = "312 *   ((91+2) /3)";

    if (0 != parser_parse(input_str))
    {
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}