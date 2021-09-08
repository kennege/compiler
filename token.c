#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "utils.h"
#include "token.h"

#define DISPLAY_BUFF (32)

struct token 
{
    char *value;
    char *type;
    char *display;

    struct token *next;
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

struct token *token_create(const char *type, const char *value, int value_len)
{
    struct token *token;

    if (NULL == type || NULL == value)
    {
        DEBUG;
        return NULL;
    }

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

    token->next = NULL;

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

int token_list_append(struct token *new, struct token **list_head)
{
    struct token *last;

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
    while (NULL != last->next)
    {
        last = last->next;
    }

    last->next = new;

    return 0;
}

int token_list_create_and_append(const char *type, const char *value, int length, struct token **list_head)
{
    struct token *new;

    new = token_create(type, value, length);
    if (NULL == new)
    {
        return -1;
    }

    return token_list_append(new, list_head);
}

int token_exists(struct token *token, struct token *list)
{
    if (NULL == token)
    {
        DEBUG;
        return -1;
    }

    while (NULL != list)
    {
        if (0 == strcmp(token->value, list->value))
        {
            return 1;
        }
        list = list->next;
    }

    return 0;
}

void token_list_step(struct token **token_list)
{
    *token_list = (*token_list)->next;    
}

const struct token *token_list_index(const struct token *token_list, int index)
{
    for (int i=0; i<index; i++)
    {
        token_list = token_list->next;
    }
    
    return token_list;
}

struct token *token_list_pop(struct token **token_list)
{
    struct token *tmp;
    tmp = *token_list;
    if (NULL != tmp)
    {
       token_list_step(token_list);
    }

    tmp->next = NULL;

    return tmp;
}

int token_compare(const struct token *token, const char *type)
{
    if (NULL == token || NULL == type)
    {
        return -1;
    }

    if (0 == strcmp(token->type, type))
    {
        return 0;
    }

    return -1;
}

int token_list_compare_all(struct token **token_list, int peak_length, ...) 
{
    va_list args;
    char *type;

    if (NULL == token_list)
    {
        DEBUG;
        return -1;
    }

    va_start(args, peak_length);
    for (int i=0; i<peak_length; i++)
    {
        type = va_arg(args, char *);
        if (0 != token_compare(token_list_index(*token_list, i), type))
        {
            return -1; 
        }
    }
    va_end(args);

    return 0;
}

int token_list_compare_any(struct token **token_list, int n_options, ...) 
{
    va_list args;
    char *type;

    if (NULL == token_list)
    {
        DEBUG;
        return -1;
    }

    va_start(args, n_options);
    for (int i=0; i<n_options; i++)
    {
        type = va_arg(args, char*);
        for (int j=0; j<n_options; j++)
        {
            if (0 == token_compare(*token_list, type))
            {
                return 0; 
            }
        }

    }
    va_end(args);

    return -1;
}

struct token *token_cpy(const struct token *token)
{
    struct token *new;

    if (NULL == token)
    {
        DEBUG;
        return NULL;
    }

    new = token_create(token->type, token->value, strlen(token->value));
    if (NULL == new)
    {
        DEBUG;
        return NULL;
    }

    return new;
}

struct token *token_list_cpy(const struct token *token_list)
{
    struct token *new;

    if (NULL == token_list)
    {
        DEBUG;
        return NULL;
    }

    new = token_cpy(token_list);
    if (NULL == new)
    {
        DEBUG;
        return NULL;
    }

    new->next = token_list_cpy(token_list->next);

    return new;
}

char *token_get_display(const struct token *token)
{
    if (NULL == token)
    {
        return NULL;
    }

    return token->display;
}

char *token_get_value(const struct token *token)
{
    if (NULL == token)
    {
        DEBUG;
        return NULL;
    }
    
    return token->value;
}

char *token_get_type(const struct token *token)
{
    if (NULL == token)
    {
        DEBUG;
        return NULL;
    }
    
    return token->type;
}

void token_print(const struct token *token)
{
    printf("%s\n", token->display);
}

void token_list_print(const struct token *token)
{
    printf("\nTOKEN LIST: \n");
    while (NULL != token)
    {
        token_print(token);
        token = token->next;
    }
    printf("\n");
}