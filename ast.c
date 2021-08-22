#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "token.h"
#include "ast.h"
#include "utils.h"
struct node *ast_destroy(struct node *ast)
{
    if (NULL == ast)
    {
        return NULL;
    }

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
    ast->type = NULL;
    ast->id = NULL;
    ast->next = NULL;

    return ast;
}

struct node *ast_binary_node_add(struct node *left, struct token *op, struct node *right)
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
    node->type = BINARY;
    node->right = right;

    return node;
}

struct node *ast_unary_node_add(struct token *op, struct node *left)
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
    node->type = UNARY;

    return node;
}

struct node *ast_value_node_set(struct token *op)
{
    struct node *node;

    node = ast_node_create();
    if (NULL == node)
    {
        return NULL;
    }
    node->op = op;
    node->set = 1;
    node->type = VALUE;

    return node;
}


struct node *ast_variable_node_add(struct token *op)
{
    struct node *node;

    node = ast_node_create();
    if (NULL == node)
    {
        return NULL;
    }
    node->op = op;
    node->set = 1;
    node->type = VARIABLE;
    node->id = token_get_value(op);

    return node;
}

struct node *ast_assignment_node_add(struct node *left, struct token *op, struct node *right)
{
    struct node *node;

    node = ast_node_create();
    if (NULL == node)
    {
        return NULL;
    }
    node->left = left;
    node->op = op;
    node->right = right;
    node->type = ASSIGNMENT;
    node->set = 1;

    return node;
}

int ast_assignment_node_append(struct node *list_head, struct node *new)
{   
    struct node *last;

    if (NULL == list_head)
    {
        list_head = new;
        return 0;
    }
    
    last = list_head;
    while (last->next != NULL)
    {
        last = last->next;
    }

    last->next = new;

    return 0;
}

const struct node *ast_assignment_node_index(const struct node *list_head, int index)
{
    for (int i=0; i<index; i++)
    {
        list_head = list_head->next;
    }

    return list_head;
}

size_t ast_assignment_node_length(const struct node *list_head)
{
    size_t length;

    length = 0;
    while (NULL != list_head)
    {
        length++;
        list_head = list_head->next;
    }

    return length;
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


