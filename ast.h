#ifndef _AST_H_
#define _AST_H_

struct node
{
    struct node *left;
    struct token *op;
    struct node *right;

    struct node *arguments;
    char *type;
    int set;

    struct next
    {
        struct node *node;
        struct token *operator;
    } *next;
};

/* NODE TYPES */
#define VALUE "VALUE"
#define UNARY "UNARY"
#define BINARY "BINARY"
#define VARIABLE "VARIABLE"
#define FUNCTION_CALL "FUNCTION_CALL"
#define FUNCTION_ARGUMENT "FUNCTION_ARGUMENT"
#define DECLARATION "DECLARATION"
#define ASSIGNMENT "ASSIGNMENT"
#define WHILE_LOOP "WHILE_LOOP"
#define FOR_LOOP "FOR_LOOP"
#define COMPARISON "COMPARISON"
#define CONDITION "CONDITION"
#define FUNCTION "FUNCTION"
#define PROGRAM "PROGRAM"

struct node *ast_node_destroy(struct node *node);
struct node *ast_destroy_all(struct node *ast);

struct node *ast_value_node_set(struct token *op);

struct node *ast_unary_node_create(struct token *op, struct node *left);
struct node *ast_variable_node_create(struct token *op);
struct node *ast_binary_node_create(struct node *left, struct token *op, struct node *right);
struct node *ast_assignment_node_create(struct node *left, struct token *op, struct node *right);
struct node *ast_declaration_node_create(struct node *left, struct token *op, struct node *right);
struct node *ast_comparison_node_create(struct token *not, struct node *left, struct token *op, struct node *right);
struct node *ast_condition_node_create(struct node *comparison_list, struct token *condition);
struct node *ast_condition_else_node_create(struct node *statement_list, struct node *condition_declaration, struct token *condition);
struct node *ast_function_argument_node_create(struct token *name);
struct node *ast_function_call_node_create(struct token *name);
struct node *ast_function_node_create(struct token *name);
struct node *ast_program_node_create(struct node *function_list);

int ast_condition_node_add_body(struct node *condition, struct node *statement_list);

int ast_function_node_add_arguments(struct node *node, struct node *arguments);
int ast_function_node_add_body(struct node *node, struct node *assignment_list);
int ast_function_node_add_return(struct node *node, struct node *return_value);
int ast_function_call_node_add_variables(struct node *node, struct node *variables);

int ast_node_append(struct node **list_head, struct node *new, struct token *operator);
int ast_node_push(struct node **list_head, struct node *new);
const struct node *ast_node_index(const struct node *list_head, int index);
size_t ast_num_nodes(const struct node *list_head);

void ast_print(const struct node *ast, int level, char *location);

#endif // _AST_H_