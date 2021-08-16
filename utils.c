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

int string_compare(const char *a, const char *b)
{
    int i;

    i = 0;
    while (a[i] != '\0' && b[i] != '\0')
    {
        if (a[i] != b[i])
        {
            return -1;
        }
        i++;
    }

    return 0;
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