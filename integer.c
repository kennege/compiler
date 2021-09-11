#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "lexer.h"
#include "token.h"
#include "integer.h"

static int integer_from_token(const struct token *token, int *in_int)
{
    char *in_str;

    in_str = token_get_value(token);
    if (NULL == in_str)
    {
        DEBUG;
        return -1;
    }

    sscanf(in_str, "%d", in_int);
    if (NULL == in_int)
    {
        DEBUG;
        return -1;
    }

    return 0;
}

static struct token *integer_to_token(int result)
{
    struct token *output;
    char *value;
    size_t size;

    size = snprintf(NULL, 0, "%d", result) + 1;
    
    value = malloc(size * sizeof(*value));
    if (NULL == value)
    {
        DEBUG;
        return NULL;
    }
    
    snprintf(value, size, "%d", result);

    output = token_create(INT, value, size-1);
    if (NULL == output)
    {
        DEBUG;
        return NULL;
    }
    
    free(value);
    return output;
}

static struct token *integer_add(int l_int, int r_int)
{
    struct token *output;

    output = integer_to_token(l_int + r_int);
    if (NULL == output)
    {
        DEBUG;
        return NULL;
    }

    return output;
}

static struct token *integer_subtract(int l_int, int r_int)
{
    struct token *output;

    output = integer_to_token(l_int - r_int);
    if (NULL == output)
    {
        DEBUG;
        return NULL;
    }

    return output;
}

static struct token *integer_multiply(int l_int, int r_int)
{
    struct token *output;

    output = integer_to_token(l_int * r_int);
    if (NULL == output)
    {
        DEBUG;
        return NULL;
    }

    return output;
}

static struct token *integer_divide(int l_int, int r_int)
{
    struct token *output;

    output = integer_to_token(l_int / r_int);
    if (NULL == output)
    {
        DEBUG;
        return NULL;
    }

    return output;
}

static struct token *integer_pass(int in_int)
{
    struct token *output;

    output = integer_to_token(in_int);
    if (NULL == output)
    {
        DEBUG;
        return NULL;
    }

    return output;
}

static struct token *integer_negate(int in_int)
{
    struct token *output;

    output = integer_to_token(-in_int);
    if (NULL == output)
    {
        DEBUG;
        return NULL;
    }

    return output;
}

static struct token *integer_and(int l_int, int r_int)
{
    struct token *output;

    output = integer_to_token(l_int && r_int);
    if (NULL == output)
    {
        DEBUG;
        return NULL;
    }

    return output;
}

static struct token *integer_or(int l_int, int r_int)
{
    struct token *output;

    output = integer_to_token(l_int || r_int);
    if (NULL == output)
    {
        DEBUG;
        return NULL;
    }

    return output;
}

static struct token *integer_not(int in_int)
{
    struct token *output;

    output = integer_to_token(!in_int);
    if (NULL == output)
    {
        DEBUG;
        return NULL;
    }

    return output;
}

static struct token *integer_equal_to(int l_int, int r_int)
{
    struct token *output;

    output = integer_to_token(l_int == r_int);
    if (NULL == output)
    {
        DEBUG;
        return NULL;
    }

    return output;
}

static struct token *integer_greater_than(int l_int, int r_int)
{
    struct token *output;

    output = integer_to_token(l_int > r_int);
    if (NULL == output)
    {
        DEBUG;
        return NULL;
    }

    return output;
}

static struct token *integer_greater_equal_to(int l_int, int r_int)
{
    struct token *output;

    output = integer_to_token(l_int >= r_int);
    if (NULL == output)
    {
        DEBUG;
        return NULL;
    }

    return output;
}

static struct token *integer_less_than(int l_int, int r_int)
{
    struct token *output;

    output = integer_to_token(l_int < r_int);
    if (NULL == output)
    {
        DEBUG;
        return NULL;
    }

    return output;
}

static struct token *integer_less_equal_to(int l_int, int r_int)
{
    struct token *output;

    output = integer_to_token(l_int <= r_int);
    if (NULL == output)
    {
        DEBUG;
        return NULL;
    }

    return output;
}

struct token *integer_binary_operations(const struct token *l_token, const struct token *r_token, const char *op)
{
    int l_int, r_int;
    struct token *output;

    struct {
        char *op;
        struct token *(*fn)(int l_int, int r_int);
    } valid_operations[] = {
        { .op = PLUS, .fn = integer_add },
        { .op = MINUS, .fn = integer_subtract },
        { .op = MULTIPLY, .fn = integer_multiply },
        { .op = DIVIDE, .fn = integer_divide },
    };

    if (0 != integer_from_token(l_token, &l_int) || 0 != integer_from_token(r_token, &r_int))
    {
        DEBUG;
        return NULL;
    }

    for (int i=0; i < LENGTH(valid_operations); i++)
    {
        if (0 == strcmp(valid_operations[i].op, op))
        {
            output = valid_operations[i].fn(l_int, r_int);
            if (NULL == output)
            {
                DEBUG;
                return NULL;
            }
            return output;
        }
    }

    DEBUG;
    return NULL;
}

struct token *integer_comparisons(const struct token *l_token, const struct token *r_token, const char *comp)
{
    int l_int, r_int;
    struct token *output;

    struct {
        char *comp;
        struct token *(*fn)(int l_int, int r_int);
    } valid_comparisons[] = {
        { .comp = AND, .fn = integer_and },
        { .comp = OR, .fn = integer_or },
        { .comp = EQUIVALENT, .fn = integer_equal_to },
        { .comp = GREATER_THAN, .fn = integer_greater_than },
        { .comp = GREATER_EQUAL, .fn = integer_greater_equal_to },
        { .comp = LESS_THAN, .fn = integer_less_than },
        { .comp = LESS_EQUAL, .fn = integer_less_equal_to },
    };

    if (0 != integer_from_token(l_token, &l_int) || 0 != integer_from_token(r_token, &r_int))
    {
        DEBUG;
        return NULL;
    }

    for (int i=0; i < LENGTH(valid_comparisons); i++)
    {
        if (0 == strcmp(valid_comparisons[i].comp, comp))
        {
            output = valid_comparisons[i].fn(l_int, r_int);
            if (NULL == output)
            {
                DEBUG;
                return NULL;
            }
            return output;
        }
    }

    DEBUG;
    return NULL;
}

struct token *integer_unary_operations(const struct token *token, const char *op)
{
    int in_int;
    struct token *output;

    struct {
        char *op;
        struct token *(*fn)(int in_int);
    } valid_operations[] = {
        { .op = PLUS, .fn = integer_pass },
        { .op = MINUS, .fn = integer_negate },
        { .op = NOT, .fn = integer_not },
    };

    if (0 != integer_from_token(token, &in_int))
    {
        DEBUG;
        return NULL;
    }

    for (int i=0; i < LENGTH(valid_operations); i++)
    {
        if (0 == strcmp(valid_operations[i].op, op))
        {
            output = valid_operations[i].fn(in_int);
            if (NULL == output)
            {
                DEBUG;
                return NULL;
            }
            return output;
        }
    }

    DEBUG;
    return NULL;
}