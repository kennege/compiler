#ifndef _TOKEN_H_
#define _TOKEN_H_

struct token;

struct token *token_create(const char *type, const char *value, int value_len);
struct token *token_destroy(struct token *token);

char *token_get_value(const struct token *token);
char *token_get_type(const struct token *token);
char *token_get_display(const struct token *token);
int token_compare(const struct token *token, const char *type);

int token_list_compare_all(struct token **token_list, int peak_length, ...);
int token_list_compare_any(struct token **token_list, int peak_length, ...);
struct token *token_cpy(const struct token *token);
int token_exists(struct token *token, struct token *list);

struct token *token_list_pop(struct token **token_list);
struct token *token_list_index(struct token *token_list, int index);
struct token **token_list_step(struct token **token_list);
int token_list_append(struct token *new, struct token **list_head);
int token_list_create_and_append(const char *type, const char *value, int length, struct token **list_head);
struct token *token_list_destroy(struct token *token_list);

void token_list_print(const struct token *token_list);

#endif // _TOKEN_H_