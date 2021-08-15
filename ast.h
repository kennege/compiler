#ifndef _AST_H_
#define _AST_H_

struct node *ast_destroy(struct node *ast);

struct node *ast_node_add(struct node *left, struct token *op, struct node *right);
struct node *ast_node_set(struct token *op);

void ast_print(struct node *ast, int level, char *location);

#endif // _AST_H_