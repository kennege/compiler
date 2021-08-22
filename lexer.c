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

static const struct {
    char *keyword;
} reserved_keywords[] = {
    { ASSIGN },
    { CONST },
    { FUNC },
    { VAR },
};

static const struct {
    char *type;
} variable_types[] = {
    { INT },
    { BOOL },
    { FLOAT },
    { STRING },
};

static const struct {
    char *character;
} reserved_characters[] = {
    { PLUS },
    { MINUS },
    { MULTIPLY },
    { DIVIDE },
    { LPAREN },
    { RPAREN },
    { LBRACE },
    { RBRACE },
    { WHITESPACE },
    { EQUALS },
};

static int lexer_keyword_match(const char *input_str, const char *str, struct window *window)
{
    int n_chars, str_ind;

    str_ind = 0;
    n_chars = string_len(str);
    for (int i=window->right; i<window->right + n_chars; i++)
    {
        if (input_str[i] != str[str_ind])
        {
            return -1;
        }
        str_ind++;
    }
    window->right += n_chars - 1;

    return 0;
}

static char *lexer_is_reserved_keyword(const char *input_str, struct window *window)
{
    for (int i=0; i<LENGTH(reserved_keywords); i++)
    {
        if (0 == lexer_keyword_match(input_str, reserved_keywords[i].keyword, window))
        {
            return reserved_keywords[i].keyword;
        }
    }

    return NULL;
}

static char *lexer_is_reserved_character(const char *input_str, struct window *window)
{
    for (int i=0; i<LENGTH(reserved_characters); i++)
    {
        if (input_str[window->left] == reserved_characters[i].character[0])
        {
            return reserved_characters[i].character;
        }
    }

    return NULL;
}

static char *lexer_is_special_character(const char *input_str, struct window *window)
{
    if (input_str[window->left] == NEWLINE[0])
    {
        return NEWLINE_TYPE;
    }
    if (input_str[window->left] == EOFILE[0])
    {
        return EOFILE_TYPE;
    }

    return NULL;
}

static char *lexer_is_variable_type(const char *input_str, struct window *window)
{
    for (int i=0; i<LENGTH(variable_types); i++)
    {
        if (0 == lexer_keyword_match(input_str, variable_types[i].type, window))
        {
            return VARTYPE;
        }
    }

    return NULL; 
}

static char *lexer_is_number(const char *input_str, struct window *window)
{
    int n_ints;

    n_ints = string_len(INTEGER);
    for (int i=0; i<n_ints; i++)
    {
        if (input_str[window->right] == INTEGER[i])
        {
            window->right++;
            if (NULL == lexer_is_number(input_str, window))
            {
                window->right--;    
            }
            return INTEGER;
        }
    }

    return NULL;
}

static char *lexer_is_word(const char *input_str, struct window *window)
{
    int n_chars;

    n_chars = strlen(CHARACTER);
    for (int i=0; i<n_chars; i++)
    {
        if (input_str[window->right] == CHARACTER[i])
        {
            window->right++;
            if (NULL == lexer_is_word(input_str, window))
            {
                window->right--;    
            }
            return WORD;
        }
    }
                
    return NULL;
}

static char *lexer_get_token_type(const char *input_str, struct window *window)
{
    char *type;

    static const struct {
        char *(*fn)(const char *input_str, struct window *window);
    } token_types[] = {
        { lexer_is_number },
        { lexer_is_reserved_character },
        { lexer_is_special_character },
        { lexer_is_reserved_keyword },
        { lexer_is_variable_type },
        { lexer_is_word },
    };

    for (int i=0; i<LENGTH(token_types); i++)
    {
        type = token_types[i].fn(input_str, window);
        if (NULL != type)
        {
            return type;
        }
    }

    DEBUG;
    fprintf(stderr, "Didnt recognise %c at character %d\n",input_str[window->left], window->left);
    return NULL;
}

struct token *lexer_lex(char *input_str)
{
    struct window window;
    struct token *token_list;
    char *type;
    int n_chr, str_size;

    window.left = window.right = n_chr = 0;
    str_size = strlen(input_str) + 1;
    token_list = NULL;
    while (window.right < str_size)
    {
        type = lexer_get_token_type(input_str, &window);      
        if (NULL == type)
        {
            DEBUG;
            return token_list_destroy(token_list);
        }

        if (0 == strcmp(WHITESPACE, type))
        {
            window.left++;
            window.right++;
            continue;
        } 

        if (0 != token_list_append(type, &input_str[window.left], window.right - window.left + 1, &token_list))
        {
            DEBUG;
            return token_list_destroy(token_list);
        };

        window.right++;
        window.left = window.right;
    }

    return token_list;
}