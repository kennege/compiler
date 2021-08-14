#ifndef _TOKEN_H_
#define _TOKEN_H_

struct token;

struct window
{
    int left;
    int right;
};

struct token *token_create();
int token_set(struct token *token, const char *type, const char *input_str, int value_len);
struct token *token_destroy(struct token *token);
void token_print(const struct token *head);

struct token *token_add(struct token *a, struct token *b);
struct token *token_subtract(struct token *a, struct token *b);
struct token *token_multiply(struct token *a, struct token *b);
struct token *token_divide(struct token *a, struct token *b);

#endif // _TOKEN_H_