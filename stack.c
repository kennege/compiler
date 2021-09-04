#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "token.h"
#include "stack.h"

struct stack
{
    struct token *var_name; // TOKEN(STRING, STRING)
    struct token *var; // TOKEN(* , *)

    struct stack *next;
};

static struct stack *stack_destroy(struct stack *stack)
{
    if (NULL == stack)
    {
        return NULL;
    }

    if (NULL != stack->var_name)
    {
        stack->var_name = token_destroy(stack->var_name);
    }

    if (NULL != stack->var)
    {
        stack->var = token_destroy(stack->var);
    }

    free(stack);

    return NULL;
}

static struct stack *stack_create(const struct token *var_name, const struct token *var)
{
    struct stack *new;

    new = malloc(sizeof(*new));
    if (NULL == new)
    {
        DEBUG;
        return NULL;
    }
    memset(new, 0, sizeof(*new));

    new->var_name = token_cpy(var_name);
    if (NULL == new->var_name)
    {
        DEBUG;
        return stack_destroy(new);
    }

    if (NULL != var)
    {
        new->var = token_cpy(var);
        if (NULL == new->var)
        {
            DEBUG;
            return NULL;
        }
    }

    new->next = NULL;

    return new;
}

struct stack *stack_destroy_all(struct stack *stack)
{
    struct stack *current, *next;

    current = stack;
    while (current != NULL)
    {
        next = current->next;
        current = stack_destroy(current);
        current = next;
    }

    stack = NULL;

    return NULL;
}

struct token *stack_extract(const struct stack *stack, const struct token *var_name)
{
    while (NULL != stack)
    {
        if (0 == strcmp(token_get_value(stack->var_name), token_get_value(var_name)))
        {
            return stack->var;
        }
        stack = stack->next;
    }   

    return NULL;
}

static int stack_insert(struct stack *stack, const struct token *var_name, struct token *var)
{
    while (NULL != stack)
    {
        if (0 == strcmp(token_get_value(stack->var_name), token_get_value(var_name)))
        {
            stack->var = token_cpy(var);
            if (NULL == stack->var)
            {
                return -1;
            }
            return 0;
        }
        stack = stack->next;
    }

    return -1;
}

int stack_append(struct stack **list_head, const struct token *var_name, struct token *var)
{   
    struct stack *new;
    struct stack *last;

    if (0 == stack_insert(*list_head, var_name, var))
    {
        return 0;
    }

    new = stack_create(var_name, var);
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

int stack_destroy_local(struct stack **list_head, const struct token *scope_name)
{
    struct stack *current, *next;

    if (NULL == *list_head)
    {
        return -1;
    }
        
    current = *list_head;
    while (NULL != current && 0 != strcmp(token_get_value(current->var_name), token_get_value(scope_name)))  
    {
        next = current->next;   
        current = stack_destroy(current);
        current = next;
    }

    *list_head = current;

    return 0;
}

void stack_print(const struct stack *stack)
{
    printf("\nstack: \n");
    while (NULL != stack)
    {
        printf("%s = %s\n", token_get_display(stack->var_name), token_get_display(stack->var));
        stack = stack->next;
    }
    printf("\n");
}