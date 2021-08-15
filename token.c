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

    int id;
    char *type;

    struct token *next;
};

static struct token *token_destroy(struct token *token)
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
static struct token *token_create()
{
    struct token *token;

    token = malloc(sizeof(*token));
    if (NULL == token)
    {
        DEBUG;
        return NULL;
    }
    token->value = malloc(sizeof(token->value));
    if (NULL == token->value)
    {
        DEBUG;
        free(token);
        return NULL;
    }
    token->id = 0;
    token->value->num = 0;
    token->value->chr = NULL;
    token->type = NULL;

    token->next = NULL;

    return token;
}

static void token_push(struct token **head, struct token *new)
{
    // TODO: add to AST
    if (NULL != *head)
    {
        new->id = (*head)->id + 1;
    }
    else
    {
        new->id = 0;
    }
    
    new->next = *head;
    *head = new;
}

static void token_print(const struct token *token)
{
    printf("TOKEN(%s, %s)\n", token->type, token->value->chr);
}

int token_save(const char *type, const char *value, int value_len, struct token **head)
{
    struct token *token;

    token = token_create();
    if (NULL == token)
    {
        DEBUG;
        return -1;
    }
    
    token->value->chr = string_cpy(value, value_len);
    if (NULL == token->value->chr)
    {
        DEBUG;
        token = token_destroy(token);
        return -1;
    }

    token->type = string_cpy(type, strlen(type));
    if (NULL == token->type)
    {
        DEBUG;
        token = token_destroy(token);
        return -1;
    }

    token_push(head, token);
    token_print(token);

    return 0;
}