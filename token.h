#ifndef _TOKEN_H_
#define _TOKEN_H_

struct token;

struct token *token_list_destroy(struct token *token_list);
int token_list_append(char *type, char *value, int length, struct token **list_head);
int token_type_compare(const struct token *token, const char *type);
struct token *token_list_pop(struct token **token_list);
char *token_get_value(const struct token *token);

void token_list_display(const struct token *token);
char *token_get_display(const struct token *token);

#endif // _TOKEN_H_