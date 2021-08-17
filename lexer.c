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
    { INT },
    { BOOL },
    { FLOAT },
    { STRING },
    { EOFILE },
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
    { WHITESPACE },
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

static char *lexer_is_number(const char *input_str, struct window *window)
{
    char *ints = INTEGER;
    int n_ints;

    n_ints = string_len(ints);
    for (int i=0; i<n_ints; i++)
    {
        if (input_str[window->right] == ints[i])
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

static char *lexer_is_variable_name(const char *input_str, struct window *window)
{
    int input_str_len, num_chars, i;

    if ((NULL != lexer_is_reserved_keyword(input_str, window)) || 
        (NULL != lexer_is_reserved_character(input_str, window)) ||
        (NULL != lexer_is_number(input_str, window)))
    {
        return NULL;
    }

    input_str_len = strlen(&input_str[window->left]);
    num_chars = strlen(CHARACTERS);
    i = 0;
    while (i < input_str_len)
    {
        for (int j=0; j<num_chars; j++)
        {
            if (input_str[window->left + i] == CHARACTERS[j])
            {
                i++;
                window->right++;
                continue;
            }
            return VAR;
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
        { lexer_is_reserved_keyword },
        { lexer_is_variable_name },
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
    printf("Didnt recognise %c\n",input_str[window->left]);
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