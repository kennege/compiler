#ifndef _TOKEN_H_
#define _TOKEN_H_

struct token;

struct token *token_list_destroy(struct token *token_list);
int token_list_append(char *type, char *value, int length, struct token **list_head);
char *token_type(const struct token *token);
struct token *token_list_pop(struct token **token_list);

void token_list_display(const struct token *token);
char *token_get_display(const struct token *token);

#endif // _TOKEN_H_