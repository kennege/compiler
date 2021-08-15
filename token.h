#ifndef _TOKEN_H_
#define _TOKEN_H_

struct token;

struct token *token_create();
struct token *token_destroy(struct token *token);
struct token *token_generate(const char *type, const char *input_str, int value_len);
char *token_display(struct token *token);

#endif // _TOKEN_H_