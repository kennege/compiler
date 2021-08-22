#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "token.h"
#include "parser.h"
#include "lexer.h"
#include "ast.h"
#include "translator.h"
#include "test.h"

int interpreter(char *input_str, FILE *fp)
{
    struct token *token_list;
    struct node *ast;
    int result;

    token_list = lexer_lex(input_str);
    if (NULL == token_list)
    {
        exit(1);
    }

    ast = parser_parse(token_list);
    if (NULL == ast)
    {
        exit(1);
    }
    // token_list_print(token_list);
    // ast_print(ast, 0, "root");
    
    result = translator_translate(ast, fp);

    token_list_destroy(token_list);
    ast = ast_destroy(ast);

    return result;
}

int main() 
{
    /* TODO: read file, lex file, parse tokens to AST, convert AST to machine code, 
    save machine code to file */
    FILE *fp;
    int test_result;

    fp = fopen("/home/gerard/GIT/compiler/compiler.s", "w");
    if (NULL == fp)
    {
        return EXIT_FAILURE;
    }

    test_result = unit_test(interpreter, fp);
    if (0 != test_result)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}