#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "token.h"
#include "lexer.h"
#include "utils.h"

struct token 
{
    union value
    {
        char *chr;
        int num;
    } *value;

    char *type;
};

struct token *token_create()
{
    struct token *token;

    token = malloc(sizeof(*token));
    if (NULL == token)
    {
        return NULL;
    }
    token->value = malloc(sizeof(token->value));
    if (NULL == token->value)
    {
        free(token);
        return NULL;
    }
    token->value->num = 0;
    token->value->chr = NULL;
    token->type = NULL;

    return token;
}

int token_set(struct token *token, const char *type, const char *value, int value_len)
{
    char *q;
    
    if (0 == strcmp(INTEGER, type))
    {
        token->value->num = strtol(value, &q, 10);
    }
    else
    {
        token->value->chr = string_cpy(value, value_len);
        if (NULL == token->value->chr)
        {
            token = token_destroy(token);
            DEBUG;
            return -1;
        }
    }     
    
    token->type = string_cpy(type, strlen(type));
    if (NULL == token->type)
    {
        token = token_destroy(token);
        DEBUG;
        return -1;
    }

    token_print(token);
    return 0;
}

struct token *token_destroy(struct token *token)
{
    if (NULL == token)
    {
        return NULL;
    }
    
    if (0 == token->value->num)
    {
        free(token->value->chr);
    }

    if (NULL != token->value)
    {
        free(token->value);
    }

    if (NULL != token->type)
    {
        free(token->type);
    }

    free(token);
    
    return NULL;
}

struct token *token_add(struct token *a, struct token *b)
{
    printf("%d + %d\n",a->value->num, b->value->num);
    a->value->num = a->value->num + b->value->num;
    b = token_destroy(b);

    return a;
}

struct token *token_subtract(struct token *a, struct token *b)
{
    printf("%d - %d\n",a->value->num, b->value->num);
    a->value->num = a->value->num - b->value->num;
    b = token_destroy(b);

    return a;
}

struct token *token_multiply(struct token *a, struct token *b)
{
    printf("%d * %d\n",a->value->num, b->value->num);
    a->value->num = a->value->num * b->value->num;
    b = token_destroy(b);

    return a;
}

struct token *token_divide(struct token *a, struct token *b)
{
    printf("%d / %d\n",a->value->num, b->value->num);
    a->value->num = a->value->num / b->value->num;
    b = token_destroy(b);

    return a;
}

void token_print(const struct token *token)
{
    if (0 == strcmp(INTEGER, token->type))
    {
        printf("TOKEN(%s, %d)\n", token->type, token->value->num);    
    }
    else
    {
        printf("TOKEN(%s, %s)\n", token->type, token->value->chr);
    }
}