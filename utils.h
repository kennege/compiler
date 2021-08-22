#ifndef _UTILS_H_
#define _UTILS_H_

char *string_cpy(const char *input_str, size_t len);
size_t string_len(const char *str);
char *string_cpycat(const char *fmt, ...);
int num_digits(int input_num);

#define LENGTH(x) (sizeof(x)/sizeof(x[0]))
#define DEBUG (fprintf(stderr, "ERROR: %s, %d\n", __FILE__, __LINE__))
#define VARNAME(var) #var

#endif // _UTILS_H_