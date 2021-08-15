#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "token.h"

#define DISPLAY_BUFF 40
struct token 
{
    char *value;
    char *type;
    char *display;
};

struct token *token_destroy(struct token *token)
{
    if (NULL == token)
    {
        return NULL;
    }

    if (NULL != token->value)
    {
        free(token->value);
    }

    if (NULL != token->type)
    {
        free(token->type);
    }

    if (NULL != token->display)
    {
        free(token->display);
    }

    free(token);
    
    return NULL;
}
struct token *token_create()
{
    struct token *token;

    token = malloc(sizeof(*token));
    if (NULL == token)
    {
        DEBUG;
        return NULL;
    }
    memset(token, 0, sizeof(*token));
    
    token->value = malloc(sizeof(token->value));
    if (NULL == token->value)
    {
        DEBUG;
        return token_destroy(token);
    }

    token->display = malloc(DISPLAY_BUFF * sizeof(token->display[0]));
    if (NULL == token->display)
    {
        DEBUG;
        return token_destroy(token);
    }

    token->value = NULL;
    token->type = NULL;

    return token;
}

struct token *token_generate(const char *type, const char *value, int value_len)
{
    struct token *token;

    token = token_create();
    if (NULL == token)
    {
        DEBUG;
        return NULL;
    }
    
    token->value = string_cpy(value, value_len);
    if (NULL == token->value)
    {
        DEBUG;
        return token_destroy(token);
    }

    token->type = string_cpy(type, strlen(type));
    if (NULL == token->type)
    {
        DEBUG;
        return token_destroy(token);
    }


    // snprintf(token->display, DISPLAY_BUFF, "TOKEN(%s, %s)", token->type, token->value);
    snprintf(token->display, DISPLAY_BUFF, "%s", token->value);

    return token;
}

char *token_display(struct token *token)
{
    return token->display;
}