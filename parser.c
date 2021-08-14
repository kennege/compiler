#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "token.h"
#include "lexer.h"
#include "parser.h"
#include "utils.h"

struct token *term(const char *input_str, struct window *window, struct token *result);
struct token *expression(const char *input_str, struct window *window, struct token *result);

struct token *factor(const char *input_str, struct window *window, struct token *result)
{
    /* factor : INTEGER | LPAREN expr RPAREN */
    struct token *expr, *rparen;

    expr = lexer_eat(INTEGER, input_str, window);
    if (NULL != expr)
    {
        result = expr;
        return result;
    }  
 
    expr = lexer_eat(LPAREN, input_str, window);
    if (NULL != expr)
    {
        result = expression(input_str, window, result);
        rparen = lexer_eat(RPAREN, input_str, window);
        if (NULL != rparen && NULL != result)
        {
            rparen = token_destroy(rparen);
            return result;
        }
    }

    DEBUG;
    expr = token_destroy(expr);
    return result;  
}

struct token *term(const char *input_str, struct window *window, struct token *result)
{   
    /* term : factor ((MULTIPLY | DIVIDE) factor) * */
    struct token *token;

    result = factor(input_str, window, result);

    while (1)
    {
        token = lexer_eat(MULTIPLY, input_str, window);
        if (NULL != token && NULL != result)
        {
            result = token_multiply(result, factor(input_str, window, result));
            continue;
        }
        token = lexer_eat(DIVIDE, input_str, window);
        if (NULL != token && NULL != result)
        {
            result = token_divide(result, factor(input_str, window, result));
            continue;
        }
        break;
    }

    token = token_destroy(token);
    return result;
}

struct token *expression(const char *input_str, struct window *window, struct token *result)
{
    /* expr : term ((PLUS | MINUS) term) * */

    struct token *token;

    result = term(input_str, window, result);

    while (1)
    {
        token = lexer_eat(PLUS, input_str, window);
        if (NULL != token && NULL != result)
        {
            result = token_add(result, term(input_str, window, result));
            continue;
        }
        token = lexer_eat(MINUS, input_str, window);
        if (NULL != token && NULL != result)
        {
            result = token_subtract(result, term(input_str, window, result));
            continue;
        }
        break;
    }

    token = token_destroy(token);
    return result;
}

int parser_parse(const char *input_str, FILE *assembly_file)
{    
    struct window window;
    struct token *result, *eof;

    window.left = window.right = 0;

    result = token_create();
    if (NULL == result)
    {
        return -1;
    }

    result = expression(input_str, &window, result);
    
    eof = token_create();
    if (NULL == eof)
    {
        result = token_destroy(result);
        return -1;
    }
    token_set(eof, EOFILE, &input_str[window.left], 1);
    
    token_print(result);

    result = token_destroy(result);
    eof = token_destroy(eof);

    return 0;
}