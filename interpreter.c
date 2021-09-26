#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "token.h"
#include "parser.h"
#include "lexer.h"
#include "ast.h"
#include "translator.h"
#include "utils.h"
#include "test.h"

static struct token *interpreter(char *input_str)
{
    struct token *token_list, *result;
    struct node *ast;

    token_list = lexer_lex(input_str);
    if (NULL == token_list)
    {
        fprintf(stderr, "ERROR: Empty program detected.\n");
        return NULL;
    }

    ast = parser_parse(token_list);
    if (NULL == ast)
    {
        fprintf(stderr, "ERROR: Empty program detected.\n");
        return NULL;
    }
    
    result = translator_translate(ast);

    token_list = token_list_destroy(token_list);
    // ast = ast_destroy_all(ast);

    return result;
}

static char *read_file(char *path)
{
    FILE *fp;
    size_t f_size;
    char *program;

    fp = fopen(path, "r");
    if (NULL == fp)
    {
        ERROR_MESSAGE;
        return NULL;
    }

    fseek(fp, 0L, SEEK_END);
    f_size = ftell(fp);
    rewind(fp);

    program = malloc((f_size + 1) * sizeof(*program));
    if (NULL == program)
    {
        ERROR_MESSAGE;
        return NULL;
    }

    for (int i=0; i <f_size; i++)
    {
        program[i] = fgetc(fp);
    }

    fclose(fp);

    return program;    
}

static void usage(char *name)
{
    fprintf(stderr, "usage: %s <program>.gl | -h | -t [-d]\n\n", name);
    fprintf(stderr, "options:\n \
        -h : help\n \
        -t : test\n");
        // -d : show debug messages\n
}

int main(int argc, char **argv) 
{
    int test_result;
    char *program;
    struct token *output;

    if (2 != argc)
    {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (0 == strcmp(argv[1], "-h"))
    {
        usage(argv[0]);
        return EXIT_SUCCESS;
    }
    else if (0 == strcmp(argv[1], "-t"))
    {
        test_result = unit_test(interpreter);
        if (0 != test_result)
        {
            return EXIT_FAILURE;
        }
    }
    else if (NULL != strstr(argv[1], ".gl"))
    {
        program = read_file(argv[1]);
        if (NULL == program)
        {
            ERROR_MESSAGE;
            return EXIT_FAILURE;
        }
        output = interpreter(program);
        if (NULL == output)
        {
            ERROR_MESSAGE;
            return EXIT_FAILURE;
        }
        printf("%s\n", token_get_value(output));
    }
    else
    {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}