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
    { WHITESPACE },
    { DIVIDE },
    { LPAREN },
    { RPAREN },
    { MINUS },
    { DOT },
    { PLUS },
    { LBRACE },
    { RBRACE },
    { EQUALS },
    { MULTIPLY },
};

static int lexer_keyword_match(const char *input_str, const char *str, struct window *window)
{
    int n_chars, str_ind;

    str_ind = 0;
    n_chars = strlen(str);
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

static char *lexer_return_float_or_int(const char *input_str, struct window *window)
{
    for (int i = window->left; i < window->right; i++)
    {
        if (input_str[i] == DOT[0])
        {
            return FLOAT;
        }
    }
    return INT;
}

static char *lexer_is_number(const char *input_str, struct window *window)
{
    int n_digits;

    n_digits = strlen(NUMBERS);
    for (int i=0; i<n_digits; i++)
    {
        if (input_str[window->right] == NUMBERS[i] || input_str[window->right] == DOT[0])
        {
            window->right++;
            if (NULL == lexer_is_number(input_str, window))
            {
                window->right--;    
            }
            return lexer_return_float_or_int(input_str, window);
        }
    }

    return NULL;
}

static char *lexer_is_string(const char *input_str, struct window *window)
{
    int n_chars;

    n_chars = strlen(CHARACTERS);
    for (int i=0; i<n_chars; i++)
    {
        if (input_str[window->right] == CHARACTERS[i])
        {
            window->right++;
            if (NULL == lexer_is_string(input_str, window))
            {
                window->right--;    
            }
            return STRING;
        }
    }
                
    return NULL;
}

static char *lexer_is_comment(const char *input_str, struct window *window)
{
    if (input_str[window->left] == COMMENT[0] && input_str[window->left + 1] == COMMENT[1])
    {
        while (input_str[window->right] != NEWLINE[0] && input_str[window->right] != EOFILE[0])
        {
            window->right++; 
        }
        window->left = window->right;
    }

    return NULL;
}

static char *lexer_get_token_type(const char *input_str, struct window *window)
{
    char *type;

    static const struct {
        char *(*fn)(const char *input_str, struct window *window);
    } token_types[] = {
        { lexer_is_comment }, // must be first to avoid confusion with DIVIDE
        { lexer_is_reserved_character },
        { lexer_is_special_character },
        { lexer_is_reserved_keyword },
        { lexer_is_variable_type },
        { lexer_is_number },
        { lexer_is_string },
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

    if (NULL == input_str)
    {
        DEBUG;
        return NULL;
    }
 
    token_list = NULL;
    window.left = window.right = 0;
    while (input_str[window.right] != EOFILE[0])
    {
        type = lexer_get_token_type(input_str, &window);      
        if (NULL == type)
        {
            DEBUG;
            return token_list_destroy(token_list);
        }

        if (0 == strcmp(type , WHITESPACE))
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