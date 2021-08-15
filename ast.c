#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "token.h"
#include "ast.h"

struct node
{
    struct node *left;
    struct token *op;
    struct node *right;

    int set;
};

struct node *ast_destroy(struct node *ast)
{
    if (NULL != ast->left)
    {
        ast->left = ast_destroy(ast->left);
    }

    if (NULL != ast->right)
    {
        ast->right = ast_destroy(ast->right);
    }

    free(ast);

    return NULL;
}

static struct node *ast_node_create()
{
    struct node *ast;

    ast = malloc(sizeof(*ast));
    if (NULL == ast)
    {
        return NULL;
    }
    memset(ast, 0, sizeof(*ast));

    ast->left = malloc(sizeof(*ast->left));
    if (NULL == ast->left)
    {
        return ast_destroy(ast);
    }
    memset(ast->left, 0, sizeof(*ast->left));

    ast->right = malloc(sizeof(*ast->right));
    if (NULL == ast->right)
    {
        return ast_destroy(ast);
    }
    memset(ast->right, 0, sizeof(*ast->right));

    ast->set = 0;

    return ast;
}

void ast_print(struct node *ast, int level, char *location)
{
    if (0 != ast->set)
    {
        printf("Level %d, %s: %s \n", level, location, token_get_display(ast->op));
    
        if (0 != ast->left->set)
        {
            printf("Level %d, %s: %s \n", level, location, token_get_display(ast->left->op));
        }
        
        if (0 != ast->right->set)
        {
            printf("Level %d, %s: %s \n", level, location, token_get_display(ast->right->op));
        }
        printf("\n");

        ast_print(ast->left, level + 1, "left");
        ast_print(ast->right, level + 1, "right");
    }
}

struct node *ast_node_add(struct node *left, struct token *op, struct node *right)
{
    struct node *node;

    node = ast_node_create();
    if (NULL == node)
    {
        return NULL;
    }
    node->left = left;
    node->op = op;
    node->set = 1;
    node->right = right;
 
    // printf("ADD: left %s, op: %s, right: %s\n", token_get_display(left->op), token_get_display(op), token_get_display(right->op));

    return node;
}

struct node *ast_node_set(struct token *op)
{
    struct node *node;

    node = ast_node_create();
    if (NULL == node)
    {
        return NULL;
    }
    node->op = op;
    node->set = 1;

    return node;
}