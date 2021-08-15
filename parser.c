#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "token.h"
#include "lexer.h"
#include "parser.h"
#include "utils.h"

int term(const char *input_str, struct window *window, struct token **head);
int expression(const char *input_str, struct window *window, struct token **head);

int factor(const char *input_str, struct window *window, struct token **head)
{
    /* factor : INTEGER | LPAREN expr RPAREN */
    if (0 == lexer_eat(INTEGER, input_str, window, head))
    {
        return 0;
    }  
 
    if (0 == lexer_eat(LPAREN, input_str, window, head))
    {
        if (0 != expression(input_str, window, head))
        {
            DEBUG;
            return -1;
        }
        if (0 != lexer_eat(RPAREN, input_str, window, head))
        {
            DEBUG;
            return -1;
        }
    }

    return 0;  
}

int term(const char *input_str, struct window *window, struct token **head)
{   
    /* term : factor ((MULTIPLY | DIVIDE) factor) * */
    factor(input_str, window, head);

    while (1)
    {
        if (0 == lexer_eat(MULTIPLY, input_str, window, head))
        {
            factor(input_str, window, head);
        }
        else if (0 == lexer_eat(DIVIDE, input_str, window, head))
        {
            factor(input_str, window, head);
        }
        break;
    }

    return 0;
}

int expression(const char *input_str, struct window *window, struct token **head)
{
    /* expr : term ((PLUS | MINUS) term) * */
    term(input_str, window, head);

    while (1)
    {
        if (0 == lexer_eat(PLUS, input_str, window, head))
        {
            term(input_str, window, head);
        }
        else if (0 == lexer_eat(MINUS, input_str, window, head))
        {
            term(input_str, window, head);
        }
        break;
    }

    return 0;
}

struct token *parser_parse(const char *input_str)
{    
    struct window window;
    struct token *head;

    head = NULL;
    window.left = window.right = 0;

    expression(input_str, &window, &head);
    
    if (0 != lexer_eat(EOFILE, input_str, &window, &head))
    {
        DEBUG;
        return NULL;
    }
    
    return head;
}