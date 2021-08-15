#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "token.h"
#include "lexer.h"
#include "utils.h"

static char *lexer_is_int(const char *input_str, struct window *window)
{
    char *ints = "0123456789";
    int n_ints;

    n_ints = strlen(ints);
    for (int i=0; i<n_ints; i++)
    {
        if (input_str[window->right] == ints[i])
        {
            window->right++;
            if (NULL == lexer_is_int(input_str, window))
            {
                window->right--;    
            }
            return INTEGER;
        }
    }

    return NULL;
}

static char *lexer_is_plus(const char *input_str, struct window *window)
{
    return (input_str[window->left] == '+') ? PLUS : NULL;
}

static char *lexer_is_minus(const char *input_str, struct window *window)
{
    return (input_str[window->left] == '-') ? MINUS : NULL;
}

static char *lexer_is_multiply(const char *input_str, struct window *window)
{
    return (input_str[window->left] == '*') ? MULTIPLY : NULL;
}

static char *lexer_is_divide(const char *input_str, struct window *window)
{
    return (input_str[window->left] == '/') ? DIVIDE : NULL;
}

static char *lexer_is_lparen(const char *input_str, struct window *window)
{
    return (input_str[window->left] == '(') ? LPAREN : NULL;
}

static char *lexer_is_rparen(const char *input_str, struct window *window)
{
    return (input_str[window->left] == ')') ? RPAREN : NULL;
}

static char *lexer_is_eof(const char *input_str, struct window *window)
{
    // TODO: change to EOF when reading file
    return (0 == strcmp(&input_str[window->left], "\0")) ? EOFILE : NULL;
}

static char *lexer_is_space(const char *input_str, struct window *window)
{
    return (input_str[window->left] == ' ') ? WHITESPACE : NULL;
}

static const struct {
    char *(*fn)(const char *input_str, struct window *window);
} tab[] = {
    { lexer_is_int },
    { lexer_is_plus },
    { lexer_is_minus },
    { lexer_is_multiply },
    { lexer_is_divide },
    { lexer_is_lparen },
    { lexer_is_rparen },
    { lexer_is_eof },
    { lexer_is_space },
};

static char *lexer_get_token_type(const char *input_str, struct window *window)
{
    char *type;

    for (int i=0; i<LENGTH(tab); i++)
    {
        type = tab[i].fn(input_str, window);
        if (NULL != type)
        {
            return type;
        }
    }

    DEBUG;
    return NULL;
}

int lexer_eat(const char *input_type, const char *input_str, struct window *window, struct token **head)
{
    char *type;
     
    while (1)
    {
        type = lexer_get_token_type(input_str, window);      
        if (NULL == type)
        {
            DEBUG;
            return -1;
        }

        if (0 == strcmp(WHITESPACE, type))
        {
            window->left++;
            window->right++;
            continue;
        }   
        
        if (0 == strcmp(type, input_type))
        {
            window->right++;
            if (0 != token_save(type, &input_str[window->left], window->right - window->left, head))
            {
                DEBUG;
                return -1;
            }
            window->left = window->right;
            return 0;
        } 

        break;
    }

    return -1;
}
