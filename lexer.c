#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "token.h"
#include "lexer.h"

struct window
{
    int left;
    int right;
};

static char *lexer_is_int(const char *input_str, struct window *window)
{
    char *ints = "0123456789";
    int n_ints;

    n_ints = string_len(ints);
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
    return (0 == string_compare(&input_str[window->left], "\0")) ? EOFILE : NULL;
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
    { lexer_is_space },
    { lexer_is_eof },
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

struct token *lexer_eat(const char *type, struct token **token_list)
{
    if (NULL == token_list)
    {
        return NULL;
    }

    if (0 == token_type_compare(*token_list, type))
    {
        return token_list_pop(token_list); 
    }

    return NULL;
}

struct token *lexer_lex(char *input_str)
{
    struct window window;
    struct token *token_list;
    char *type;
    int n_chr, len;

    window.left = window.right = n_chr = 0;
    len = string_len(input_str);
    token_list = NULL;
    while (window.right < len + 1)
    {
        type = lexer_get_token_type(input_str, &window);      
        if (NULL == type)
        {
            DEBUG;
            return token_list_destroy(token_list);
        }

        if (0 == string_compare(WHITESPACE, type))
        {
            window.left++;
            window.right++;
            continue;
        } 

        if (0 != token_list_append(type, &input_str[window.left], window.right + 1 - window.left, &token_list))
        {
            DEBUG;
            return token_list_destroy(token_list);
        };
        window.right++;
        window.left = window.right;
    }

    return token_list;
}