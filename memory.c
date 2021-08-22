#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "memory.h"

struct kv
{
    char *key;
    int value;

    struct kv *next;
};

static struct kv *memory_destroy(struct kv *memory)
{
    if (NULL == memory)
    {
        DEBUG;
        return NULL;
    }

    if (NULL != memory->key)
    {
        free(memory->key);
    }

    free(memory);

    return NULL;
}

static struct kv *memory_create(const char *variable_name, int value)
{
    struct kv *new;

    new = malloc(sizeof(*new));
    if (NULL == new)
    {
        DEBUG;
        return NULL;
    }
    memset(new, 0, sizeof(*new));

    new->key = string_cpy(variable_name, strlen(variable_name));
    if (NULL == new->key)
    {
        DEBUG;
        return memory_destroy(new);
    }

    new->value = value;
    new->next = NULL;

    return new;
}

struct kv *memory_destroy_all(struct kv *memory)
{
    struct kv *current, *next;

    current = memory;
    while (current != NULL)
    {
        next = current->next;
        current = memory_destroy(current);
        current = next;
    }

    memory = NULL;

    return NULL;
}

int memory_extract(const struct kv *kv, const char *variable_name)
{
    while (NULL != kv)
    {
        if (0 == strcmp(kv->key, variable_name))
        {
            return kv->value;
        }
        kv = kv->next;
    }   

    return -1;
}

int memory_append(struct kv **list_head, const char *variable_name, int value)
{   
    struct kv *new, *last;

    new = memory_create(variable_name, value);
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

void memory_print(const struct kv *kv)
{
    printf("\nMEMORY: \n");
    while (NULL != kv)
    {
        printf("%s = %d\n", kv->key, kv->value);
        kv = kv->next;
    }
    printf("\n");
}