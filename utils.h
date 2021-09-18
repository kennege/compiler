#ifndef _UTILS_H_
#define _UTILS_H_

char *string_cpy(const char *input_str, size_t len);
char *string_cpycat(const char *fmt, ...);

#define LENGTH(x) (sizeof(x)/sizeof(x[0]))

#define ERROR_MESSAGE (fprintf(stderr, "ERROR: %s : %s : %d\n", __FILE__, __func__, __LINE__))

#define DEBUG_PRINT (0)

#define debug_print(fmt, ...)                                                                \
        do {                                                                                 \
            if (DEBUG_PRINT)                                                                 \
            {                                                                                \
                fprintf(stderr, "debug: %s : %s : %d : ", __FILE__, __func__, __LINE__);     \
                fprintf(stderr, fmt, __VA_ARGS__);                                           \
            }                                                                                \
        } while (0)                                                                          \

#endif // _UTILS_H_