#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "lexer.h"
#include "token.h"
#include "string.h"

static int string_from_token(const struct token *token, char *in_char)
{
    char *in_str;

    in_str = token_get_value(token);
    if (NULL == in_str)
    {
        return -1;
    }

    return 0;
}

static struct token *string_to_token(char *result)
{
    struct token *output;
    char *value;
    size_t size;

    size = strlen(result) + 1;
    
    value = malloc(size * sizeof(*value));
    if (NULL == value)
    {
        return NULL;
    }
    
    snprintf(value, size-1, "%s", value);

    output = token_create(STRING, value, size-1);
    if (NULL == output)
    {
        return NULL;
    }
    
    free(value);
    return output;
}

static struct token *string_concat(const char *l_char, const char *r_char)
{
    char *sum;
    struct token *output;

    sum = string_cpycat("%s%s", l_char, r_char);
    if (NULL == sum)
    {
        return NULL;
    }

    output = string_to_token(sum);

    free(sum);
    return output;
}

struct token *string_binary_operations(const struct token *l_token, const struct token *r_token, const char *op)
{
    char l_char, r_char;
    struct token *output;

    struct {
        char *op;
        struct token *(*fn)(const char *left, const char *right);
    } valid_operations[] = {
        { .op = PLUS, .fn = string_concat },
    };

    if (0 != string_from_token(l_token, &l_char) || 0 != string_from_token(r_token, &r_char))
    {
        ERROR_MESSAGE;
        return NULL;
    }

    for (int i=0; i < LENGTH(valid_operations); i++)
    {
        if (0 == strcmp(valid_operations[i].op, op))
        {
            output = valid_operations[i].fn(&l_char, &r_char);
            if (NULL == output)
            {
                ERROR_MESSAGE;
                return NULL;
            }
            return output;
        }
    }

    ERROR_MESSAGE;
    return NULL;
}