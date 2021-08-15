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
    
    FILE *fp;
    char *input_str = "312 *   ((91+2) /3)";

    fp = fopen("/home/gerard/GIT/compiler/compiler.s", "w");
    if (NULL == fp)
    {
        return EXIT_FAILURE;
    }

    if (NULL == parser_parse(input_str))
    {
        return EXIT_FAILURE;
    }
    
    // generate assembly from head

    return EXIT_SUCCESS;
}