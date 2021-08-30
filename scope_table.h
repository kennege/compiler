#ifndef _SCOPE_TABLE_H_
#define _SCOPE_TABLE_H_

struct scope *scope_create(char *name);
struct scope *scope_destroy(struct scope *scope);
int scope_add(struct scope **scope, char *name);
struct scope *scope_get_current(struct scope **scope);
void scope_set_current(struct scope *scope, char *name);
int scope_previous_declaration(struct scope *scope, const struct token *contents);
int scope_insert(struct scope *scope, struct token *contents);
void scope_revert(struct scope **scope);
void scope_print(struct scope *scope);

#endif // _SCOPE_TABLE_H_

