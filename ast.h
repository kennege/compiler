#ifndef _AST_H_
#define _AST_H_

struct node
{
    struct node *left;
    struct token *op;
    struct node *right;

    char *type;
    int set;

    struct node *next;
};

/* NODE TYPES */
#define VALUE "VALUE"
#define UNARY "UNARY"
#define BINARY "BINARY"
#define VARIABLE "VARIABLE"
#define DECLARATION "DECLARATION"
#define ASSIGNMENT "ASSIGNMENT"
#define FUNCTION "FUNCTION"
#define PROGRAM "PROGRAM"

struct node *ast_destroy(struct node *ast);

struct node *ast_value_node_set(struct token *op);
struct node *ast_unary_node_add(struct token *op, struct node *left);
struct node *ast_variable_node_add(struct token *op);
struct node *ast_binary_node_add(struct node *left, struct token *op, struct node *right);
struct node *ast_assignment_node_add(struct node *left, struct token *op, struct node *right);
struct node *ast_declaration_node_add(struct node *left, struct token *op, struct node *right);
struct node *ast_function_node_add(struct token *name, struct node *assignment_list);
int ast_function_node_add_return(struct node *function_node, struct node *return_value);
struct node *ast_program_node_add(struct node *function_list);

int ast_node_append(struct node *list_head, struct node *new);
const struct node *ast_node_index(const struct node *list_head, int index);
size_t ast_num_nodes(const struct node *list_head);


void ast_print(const struct node *ast, int level, char *location);

#endif // _AST_H_