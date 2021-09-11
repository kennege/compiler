#ifndef _FLOAT_H_
#define _FLOAT_H_

struct token *float_binary_operations(const struct token *left, const struct token *right, const char *op);
struct token *float_unary_operations(const struct token *token, const char *op);
struct token *float_comparisons(const struct token *left, const struct token *right, const char *op);

#endif // _FLOAT_H_
