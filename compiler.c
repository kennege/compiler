#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "token.h"
#include "parser.h"
#include "lexer.h"
#include "ast.h"

int main() 
{
    /* TODO: read file, lex file, parse tokens to AST, convert AST to machine code, 
    save machine code to file */

    struct token *token_list;
    struct node *ast;
    
    FILE *fp;
    char *input_str = "12+(2+3)";

    fp = fopen("/home/gerard/GIT/compiler/compiler.s", "w");
    if (NULL == fp)
    {
        return EXIT_FAILURE;
    }

    token_list = lexer_lex(input_str);
    if (NULL == token_list)
    {
        return EXIT_FAILURE;
    }

    token_list_display(token_list);

    ast = parser_parse(token_list);
    if (NULL == ast)
    {
        return EXIT_FAILURE;
    }
    
    ast_print(ast, 0, "root");

    // generate assembly from ast

    token_list = token_list_destroy(token_list);
    ast = ast_destroy(ast);

    return EXIT_SUCCESS;
}