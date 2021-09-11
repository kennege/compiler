#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "lexer.h"
#include "token.h"
#include "float.h"

static float float_from_token(const struct token *token, float *in_float)
{
    char *str;

    str = token_get_value(token);
    if (NULL == str)
    {
        return -1;
    }

    *in_float = atof(str);
    if (NULL == in_float)
    {
        return -1;
    }

    return 0;
}

static struct token *float_to_token(float result)
{
    struct token *output;
    char *value;
    size_t size;

    size = snprintf(NULL, 0, "%.02f", result) + 1;
    
    value = malloc(size * sizeof(*value));
    if (NULL == value)
    {
        return NULL;
    }
    
    snprintf(value, size, "%.02f", result);

    output = token_create(FLOAT, value, size-1);
    if (NULL == output)
    {
        return NULL;
    }
    
    free(value);
    return output;
}

static struct token *float_add(float l_float, float r_float)
{
    struct token *output;

    output = float_to_token(l_float + r_float);
    if (NULL == output)
    {
        return NULL;
    }

    return output;
}

static struct token *float_subtract(float l_float, float r_float)
{
    struct token *output;

    output = float_to_token(l_float - r_float);
    if (NULL == output)
    {
        return NULL;
    }

    return output;
}

static struct token *float_multiply(float l_float, float r_float)
{
    struct token *output;

    output = float_to_token(l_float * r_float);
    if (NULL == output)
    {
        return NULL;
    }

    return output;
}

static struct token *float_divide(float l_float, float r_float)
{
    struct token *output;

    output = float_to_token(l_float / r_float);
    if (NULL == output)
    {
        return NULL;
    }

    return output;
}

static struct token *float_pass(float in_float)
{
    struct token *output;

    output = float_to_token(in_float);
    if (NULL == output)
    {
        return NULL;
    }

    return output;
}

static struct token *float_negate(float in_float)
{
    struct token *output;

    output = float_to_token(-in_float);
    if (NULL == output)
    {
        return NULL;
    }

    return output;
}

static struct token *float_and(float l_float, float r_float)
{
    struct token *output;

    output = float_to_token(l_float && r_float);
    if (NULL == output)
    {
        DEBUG;
        return NULL;
    }

    return output;
}

static struct token *float_or(float l_float, float r_float)
{
    struct token *output;

    output = float_to_token(l_float || r_float);
    if (NULL == output)
    {
        DEBUG;
        return NULL;
    }

    return output;
}

static struct token *float_not(float in_float)
{
    struct token *output;

    output = float_to_token(!in_float);
    if (NULL == output)
    {
        DEBUG;
        return NULL;
    }

    return output;
}

static struct token *float_equal_to(float l_float, float r_float)
{
    struct token *output;

    output = float_to_token(l_float == r_float);
    if (NULL == output)
    {
        DEBUG;
        return NULL;
    }

    return output;
}

static struct token *float_greater_than(float l_float, float r_float)
{
    struct token *output;

    output = float_to_token(l_float > r_float);
    if (NULL == output)
    {
        DEBUG;
        return NULL;
    }

    return output;
}

static struct token *float_greater_equal_to(float l_float, float r_float)
{
    struct token *output;

    output = float_to_token(l_float >= r_float);
    if (NULL == output)
    {
        DEBUG;
        return NULL;
    }

    return output;
}

static struct token *float_less_than(float l_float, float r_float)
{
    struct token *output;

    output = float_to_token(l_float < r_float);
    if (NULL == output)
    {
        DEBUG;
        return NULL;
    }

    return output;
}

static struct token *float_less_equal_to(float l_float, float r_float)
{
    struct token *output;

    output = float_to_token(l_float <= r_float);
    if (NULL == output)
    {
        DEBUG;
        return NULL;
    }

    return output;
}

struct token *float_binary_operations(const struct token *l_token, const struct token *r_token, const char *op)
{
    float l_float, r_float;
    struct token *output;

    struct {
        char *op;
        struct token *(*fn)(float l_float, float r_float);
    } valid_operations[] = {
        { .op = PLUS, .fn = float_add },
        { .op = MINUS, .fn = float_subtract },
        { .op = MULTIPLY, .fn = float_multiply },
        { .op = DIVIDE, .fn = float_divide },
    };

    if (0 != float_from_token(l_token, &l_float) || 0 != float_from_token(r_token, &r_float))
    {
        DEBUG;
        return NULL;
    }

    for (int i=0; i < LENGTH(valid_operations); i++)
    {
        if (0 == strcmp(valid_operations[i].op, op))
        {
            output = valid_operations[i].fn(l_float, r_float);
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

struct token *float_comparisons(const struct token *l_token, const struct token *r_token, const char *comp)
{
    float l_float, r_float;
    struct token *output;

    struct {
        char *comp;
        struct token *(*fn)(float l_float, float r_float);
    } valid_comparisons[] = {
        { .comp = AND, .fn = float_and },
        { .comp = OR, .fn = float_or },
        { .comp = EQUIVALENT, .fn = float_equal_to },
        { .comp = GREATER_THAN, .fn = float_greater_than },
        { .comp = GREATER_EQUAL, .fn = float_greater_equal_to },
        { .comp = LESS_THAN, .fn = float_less_than },
        { .comp = LESS_EQUAL, .fn = float_less_equal_to },
    };

    if (0 != float_from_token(l_token, &l_float) || 0 != float_from_token(r_token, &r_float))
    {
        DEBUG;
        return NULL;
    }

    for (int i=0; i < LENGTH(valid_comparisons); i++)
    {
        if (0 == strcmp(valid_comparisons[i].comp, comp))
        {
            output = valid_comparisons[i].fn(l_float, r_float);
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

struct token *float_unary_operations(const struct token *token, const char *op)
{
    float in_float;
    struct token *output;

    struct {
        char *op;
        struct token *(*fn)(float in_float);
    } valid_operations[] = {
        { .op = PLUS, .fn = float_pass },
        { .op = MINUS, .fn = float_negate },
        { .op = NOT, .fn = float_not },
    };

    if (0 != float_from_token(token, &in_float))
    {
        DEBUG;
        return NULL;
    }

    for (int i=0; i < LENGTH(valid_operations); i++)
    {
        if (0 == strcmp(valid_operations[i].op, op))
        {
            output = valid_operations[i].fn(in_float);
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