#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "token.h"

#define DISPLAY_BUFF 32
struct token 
{
    char *value;
    char *type;
    char *display;

    struct token *next;
};

static struct token *token_destroy(struct token *token)
{
    if (NULL == token)
    {
        return NULL;
    }

    if (NULL != token->value)
    {
        free(token->value);
        token->value = NULL;
    }

    if (NULL != token->type)
    {
        free(token->type);
        token->type = NULL;
    }

    if (NULL != token->display)
    {
        free(token->display);
        token->display = NULL;
    }

    free(token);
    token = NULL;
    
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
    memset(token, 0, sizeof(*token));

    token->display = malloc(DISPLAY_BUFF * sizeof(token->display[0]));
    if (NULL == token->display)
    {
        DEBUG;
        return token_destroy(token);
    }

    token->value = NULL;
    token->type = NULL;
    token->next = NULL;

    return token;
}

static struct token *token_generate(const char *type, const char *value, int value_len)
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

    snprintf(token->display, DISPLAY_BUFF, "TOKEN(%s, %s)", token->type, token->value);

    return token;
}

struct token *token_list_destroy(struct token *token_list)
{
    struct token *current, *next;

    current = token_list;
    while (current != NULL)
    {
        next = current->next;
        current = token_destroy(current);
        current = next;
    }

    token_list = NULL;

    return NULL;
}

struct token *token_list_pop(struct token **token_list)
{
    struct token *tmp;
    tmp = *token_list;
    if (NULL != tmp)
    {
        *token_list = (*token_list)->next;
    }

    return tmp;
}

int token_list_append(char *type, char *value, int length, struct token **list_head)
{
    struct token *new;
    struct token *last;

    new = token_generate(type, value, length);
    if (NULL == new)
    {
        DEBUG;
        return -1;
    }

    if (NULL == *list_head)
    {
        *list_head = new;
        return 0;
    }
    
    last = *list_head;
    while (last->next != NULL)
    {
        last = last->next;
    }

    last->next = new;

    return 0;
}

char *token_type(const struct token *token)
{
    return token->type;
}

char *token_get_display(const struct token *token)
{
    return token->display;
}

void token_list_display(const struct token *token)
{
    printf("\nTOKEN LIST: \n");
    while (NULL != token)
    {
        printf("%s\n", token->display);
        token = token->next;
    }
    printf("\n");
}