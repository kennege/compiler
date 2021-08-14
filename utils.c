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

// void debug_printf(const char *format, ...)
// {
//     char buffer[256];
//     va_list args;
//     va_start(args, format);
//     vsprintf(buffer, format, args);
//     va_end(args);
//     printf("ERROR: %s, %d %s \n", __FILE__, __LINE__, buffer);
// }