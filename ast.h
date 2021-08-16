#ifndef _AST_H_
#define _AST_H_

struct node
{
    struct node *left;
    struct token *op;
    struct node *right;

    char *type;
    int set;
};

#define BINARY "BINARY"
#define UNARY "UNARY"
#define VALUE "VALUE"

struct node *ast_destroy(struct node *ast);

struct node *ast_binary_node_add(struct node *left, struct token *op, struct node *right);
struct node *ast_unary_node_add(struct token *op, struct node *left);
struct node *ast_value_node_set(struct token *op);

void ast_print(struct node *ast, int level, char *location);

#endif // _AST_H_