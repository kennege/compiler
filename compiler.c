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

    if (0 != parser_parse(input_str, fp))
    {
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}