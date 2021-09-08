#ifndef _STACK_H_
#define _STACK_H_

struct stack;

struct stack *stack_destroy_all(struct stack *stack);
int stack_destroy_local(struct stack **list_head, const struct token *var_name);

int stack_append(struct stack **list_head, const struct token *var_name, const struct token *var);

int stack_function_declaration_append(struct stack **list_head, const struct token *func_name, const void *address);
int stack_function_arg_append(struct stack *list_head, struct token *arg_name);
const struct token *stack_get_function_arg(const struct stack *stack, struct token *func_name, int arg_index);

const struct token *stack_extract(const struct stack *stack, const struct token *var_name);
const void *stack_get_address(const struct stack *stack, struct token *func_name);

void stack_print(const struct stack *stack);

#endif // _STACK_H_
