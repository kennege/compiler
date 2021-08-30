#ifndef _STACK_H_
#define _STACK_H_

struct stack;

int stack_append(struct stack **list_head, const struct token *var_name, struct token *var);
int stack_destroy_local(struct stack **list_head, const struct token *var_name);
struct stack *stack_destroy_all(struct stack *stack);
struct token *stack_extract(const struct stack *stack, const struct token *var_name);
void stack_print(const struct stack *stack);

#endif // _STACK_H_
