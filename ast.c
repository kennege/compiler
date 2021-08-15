#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "token.h"
#include "ast.h"


struct node *ast_destroy(struct node *ast)
{
    if (NULL == ast->op)
    {
        return NULL;
    }
    ast->op = token_destroy(ast->op);

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

struct node *ast_node_create()
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

void ast_print(struct node *ast)
{
    if (ast->set)
    {
        printf("  %s \n", token_display(ast->op));
        if (ast->left->set && ast->right->set)
        {
            // printf("%s    ", token_display(ast->left->op));
            printf("    %s\n", token_display(ast->right->op));
        }
        else if (ast->left->set)
        {
            printf("%s      ", token_display(ast->left->op));
        }
        ast_print(ast->left);
        ast_print(ast->right);
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
 
    // printf("ADD: left %s, op: %s, right: %s", token_display(left->op), token_display(op), token_display(right->op));

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
    // printf("SET: %s", token_display(op));

    return node;
}