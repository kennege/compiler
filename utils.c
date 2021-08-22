#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "utils.h"

char *string_cpy(const char *input_str, size_t len)
{
    char *out_str;

    out_str = malloc((len+1) * sizeof(*out_str));
    if (NULL == out_str)
    {
        DEBUG;
        return NULL;
    }
    memcpy(out_str, input_str, len);
    out_str[len] = '\0';

    return out_str;
}

char *string_cpycat(const char *fmt, ...)
{
    char *out_str;
    size_t out_size;
    va_list args;

    if (NULL == fmt)
    {
        return NULL;
    }

    va_start(args, fmt);
    out_size = vsnprintf(NULL, 0, fmt, args) + 1;
    va_end(args);
    if (out_size <= 1)
    {
        return NULL;
    }

    out_str = malloc(out_size * sizeof(out_str[0]));
    if (NULL == out_str)
    {
        return NULL;
    }

    va_start(args, fmt);
    vsnprintf(out_str, out_size, fmt, args);
    va_end(args);

    return out_str;
}

int num_digits(int input_num)
{
    int num_digits;

    num_digits = 0;
    while (0 != input_num)
    {
        input_num /= 10;
        num_digits++;
    }
    
    return num_digits;
}

size_t string_len(const char *str)
{
    size_t i;

    i = 0;
    while (str[i] != '\0' && str[i] != '\n')
    {
        i++;
    }

    return i;
}