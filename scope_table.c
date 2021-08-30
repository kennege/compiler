#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "token.h"
#include "utils.h"
#include "scope_table.h"

struct scope
{
    char *name;
    int current;
    struct token *contents;
    struct scope *parent;

    struct scope *next;
};

struct scope *scope_destroy(struct scope *scope)
{
    struct scope *current, *next;
    
    current = scope;
    while (current != NULL)
    {
        next = current->next;

        if (NULL != current->name)
        {
            free(current->name);
        }

        if (NULL != current->contents)
        {
            current->contents = token_list_destroy(current->contents);
        }

        free(current);

        current = next;
    }

    return NULL;
}

struct scope *scope_create(char *name)
{
    struct scope *new;

    new = malloc(sizeof(*new));
    if (NULL == new)
    {
        return NULL;
    }

    new->name = string_cpy(name, strlen(name));

    if (NULL == new->name)
    {
        DEBUG;
        return scope_destroy(new);
    }

    new->current = 1;
    new->contents = NULL;
    new->parent = NULL;
    new->next = NULL;

    return new;
}

void scope_set_current(struct scope *scope, char *name)
{
    while (NULL != scope)
    {
        if (0 == strcmp(scope->name, name))
        {
            scope->current = 1;
        }
        else
        {
            scope->current = 0;
        }
        
        scope = scope->next;
    }
}

struct scope *scope_get_current(struct scope **scope)
{
    struct scope *tmp;

    tmp = *scope;
    while (NULL != tmp)
    {
        if (1 == tmp->current)  
        {
            return tmp;
        }
        tmp = tmp->next;
    }

    return NULL;
}

int scope_add(struct scope **scope, char *name)
{
    struct scope *new;

    if (NULL == name)
    {
        DEBUG;
        return -1;
    }

    new = scope_create(name);
    if (NULL == new)
    {
        DEBUG;
        return -1;
    }

    new->parent = scope_get_current(scope);
    if (NULL == new->parent)
    {
        DEBUG;
        return -1;
    }

    scope_set_current(*scope, name);

    if (NULL == *scope)
    {
        *scope = new;
        return 0;
    }

    new->next = *scope;
    *scope = new;

    return 0;
}

void scope_revert(struct scope **scope)
{
    scope_set_current(*scope, (*scope)->next->name);
}

int scope_insert(struct scope *scope, struct token *contents)
{
    if (!token_exists(contents, scope_get_current(&scope)->contents))
    {
        if (0 != token_list_append(contents, &(scope_get_current(&scope)->contents)))
        {
            return -1;
        }
    }

    return 0;
}

void scope_print(struct scope *scope)
{
    printf("\nSCOPE TABLE: \n");
    while (NULL != scope)
    {
        printf("Current scope: %s\n", scope->name);
        if (NULL != scope->parent)
        {
            printf("Parent: %s\n", scope->parent->name);
        }
        if (NULL != scope->contents)
        {
            token_list_print(scope->contents);
        }
        scope = scope->next;
    }
    printf("\n");
}