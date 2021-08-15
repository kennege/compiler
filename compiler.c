#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "parser.h"
#include "token.h"
#include "lexer.h"
#include "ast.h"

int main() 
{
    /* TODO: read file, lex file, parse tokens to AST, convert AST to machine code, 
    save machine code to file */

    struct token *token_list;
    struct node *ast;
    
    FILE *fp;
    char *input_str = "1+2+3";

    fp = fopen("/home/gerard/GIT/compiler/compiler.s", "w");
    if (NULL == fp)
    {
        return EXIT_FAILURE;
    }

    // TODO: split up lexing and parsing: parse a list of tokens
    // token_list = lexer_lex(intput_str);
    // if (NULL == token)



    ast = parser_parse(input_str);
    if (NULL == ast)
    {
        return EXIT_FAILURE;
    }
    
    ast_print(ast);


    // generate assembly from ast

    return EXIT_SUCCESS;
}