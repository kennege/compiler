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
#define ASSIGNMENT "ASSIGNMENT"

struct node *ast_destroy(struct node *ast);

struct node *ast_binary_node_add(struct node *left, struct token *op, struct node *right);
struct node *ast_unary_node_add(struct token *op, struct node *left);
struct node *ast_value_node_set(struct token *op);
struct node *ast_variable_node_add(struct token *op);
struct node *ast_assignment_node_add(struct node *left, struct token *op, struct node *right);

int ast_assignment_node_append(struct node *list_head, struct node *new);
const struct node *ast_assignment_node_index(const struct node *list_head, int index);
size_t ast_assignment_node_length(const struct node *list_head);


void ast_print(const struct node *ast, int level, char *location);

#endif // _AST_H_