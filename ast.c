#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "token.h"
#include "ast.h"
#include "utils.h"

struct node *ast_node_destroy(struct node *node)
{
    if (NULL == node)
    {
        return NULL;
    }

    free(node);
    node->set = 0;

    return NULL;
}

struct node *ast_destroy_all(struct node *ast)
{
    // struct node *current, *next;

    if (NULL == ast)
    {
        return NULL;
    }

    if (NULL != ast->left)
    {
        ast->left = ast_destroy_all(ast->left);
    }

    if (NULL != ast->right)
    {
        ast->right = ast_destroy_all(ast->right);
    }

    if (NULL != ast->arguments)
    {
        ast->arguments = ast_destroy_all(ast->arguments);
    }

    // if (NULL != ast->next->node)
    // {
    //     current = ast->next->node;
    //     while (current != NULL)
    //     {
    //         next = current;
    //         current = ast_node_destroy(current);
    //         current = next;
    //     }
    // }

    return ast_node_destroy(ast);
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
        return ast_destroy_all(ast);
    }
    memset(ast->left, 0, sizeof(*ast->left));

    ast->right = malloc(sizeof(*ast->right));
    if (NULL == ast->right)
    {
        return ast_destroy_all(ast);
    }
    memset(ast->right, 0, sizeof(*ast->right));

    ast->arguments = malloc(sizeof(*ast->arguments));
    if (NULL == ast->arguments)
    {
        return ast_destroy_all(ast);
    }
    memset(ast->arguments, 0, sizeof(*ast->arguments));

    ast->next = malloc(sizeof(*ast->next));
    if (NULL == ast->next)
    {
        return NULL;
    }
    ast->next->node = NULL;
    ast->next->operator = NULL;
    
    ast->set = 0;
    ast->type = NULL;

    return ast;
}

struct node *ast_binary_node_create(struct node *left, struct token *op, struct node *right)
{
    struct node *node;

    if (NULL == left || NULL == op || NULL == right)
    {
        return NULL;
    }

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

struct node *ast_unary_node_create(struct token *op, struct node *left)
{
    struct node *node;

    if (NULL == op || NULL == left)
    {
        return NULL;
    }

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

    if (NULL == op)
    {
        return NULL;
    }

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

struct node *ast_variable_node_create(struct token *op)
{
    struct node *node;

    if (NULL == op)
    {
        return NULL;
    }

    node = ast_node_create();
    if (NULL == node)
    {
        return NULL;
    }
    node->op = op;
    node->type = VARIABLE;
    node->set = 1;

    return node;
}

struct node *ast_assignment_node_create(struct node *left, struct token *op, struct node *right)
{
    struct node *node;

    if (NULL == left || NULL == op || NULL == right)
    {
        return NULL;
    }

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

struct node *ast_declaration_node_create(struct node *left, struct token *op, struct node *right)
{
    struct node *node;

    if (NULL == left || NULL == op || NULL == right)
    {
        return NULL;
    }

    node = ast_node_create();
    if (NULL == node)
    {
        return NULL;
    }
    node->left = left;
    node->op = op;
    node->right = right;
    node->type = DECLARATION;
    node->set = 1;

    return node;
}

struct node *ast_comparison_node_create(struct token *not, struct node *left, struct token *op, struct node *right)
{
    struct node *node;

    if (NULL == left || NULL == op || NULL == right)
    {
        return NULL;
    }

    node = ast_node_create();
    if (NULL == node)
    {
        return NULL;
    }

    if (NULL != not)
    {
        node->op = not;
    }
    else
    {
        node->op = op;
    }
    
    node->left = left;
    node->right = right;
    node->type = COMPARISON;
    node->set = 1;

    return node;
}

struct node *ast_condition_node_create(struct node *comparison_list, struct token *condition)
{
    struct node *node;

    if (NULL == comparison_list)
    {
        return NULL;
    }

    node = ast_node_create();
    if (NULL == node)
    {
        return NULL;
    }
    node->left = comparison_list;
    node->op = condition;
    node->type = CONDITION;
    node->set = 1;

    return node;
}

int ast_condition_node_add_body(struct node *condition, struct node *statement_list)
{
    if (NULL == condition || NULL == statement_list)
    {
        return -1;
    }
    
    condition->right = statement_list;

    return 0;
}

struct node *ast_condition_else_node_create(struct node *statement_list, struct node *condition_declaration, struct token *condition)
{
    struct node *node;

    if (NULL == statement_list)
    {
        return NULL;
    }

    node = ast_node_create();
    if (NULL == node)
    {
        return NULL;
    }
    node->left = condition_declaration;
    node->op = condition;
    node->right = statement_list;
    node->type = CONDITION;
    node->set = 1;

    return node;
}

struct node *ast_function_call_node_create(struct token *name)
{
    struct node *node;

    if ( NULL == name)
    {
        return NULL;
    }

    node = ast_node_create();
    if (NULL == node)
    {
        return NULL;
    }
    node->op = name;
    node->type = FUNCTION_CALL;
    node->set = 1;

    return node;
}

int ast_function_call_node_add_variables(struct node *node, struct node *variables)
{
    if (NULL == node || NULL == variables)
    {
        return -1;
    }

    node->left = variables;

    return 0;
}

struct node *ast_function_node_create(struct token *name)
{
    struct node *node;

    if (NULL == name)
    {
        return NULL;
    }

    node = ast_node_create();
    if (NULL == node)
    {
        return NULL;
    }
    node->op = name;
    node->left = NULL;
    node->right = NULL;
    node->arguments = NULL;
    node->type = FUNCTION;
    node->set = 1;

    return node;
}

struct node *ast_function_argument_node_create(struct token *name)
{
    struct node *node;

    if (NULL == name)
    {
        return NULL;
    }

    node = ast_node_create();
    if (NULL == node)
    {
        return NULL;
    }
    node->op = name;
    node->type = FUNCTION_ARGUMENT;
    node->set = 1;

    return node;
}

int ast_function_node_add_arguments(struct node *node, struct node *arguments)
{
    if (NULL == node || NULL == arguments)
    {
        return -1;
    }

    node->arguments = arguments;

    return 0;
}

int ast_function_node_add_body(struct node *node, struct node *assignment_list)
{
    if (NULL == node || NULL == assignment_list)
    {
        return -1;
    }

    node->left = assignment_list;

    return 0;
}

int ast_function_node_add_return(struct node *node, struct node *return_value)
{
    if (NULL == return_value || NULL == node)
    {
        return -1;
    }

    node->right = return_value;

    return 0;
}

struct node *ast_program_node_create(struct node *function_list)
{
    struct node *node;

    if (NULL == function_list)
    {
        return NULL;
    }

    node = ast_node_create();
    if (NULL == node)
    {
        return NULL;
    }
    node->left = function_list;
    node->type = PROGRAM;
    node->set = 1;

    return node;
}

int ast_node_append(struct node **list_head, struct node *new, struct token *operator)
{   
    struct node *last;

    if (NULL == new)
    {
        return -1;
    }

    if (NULL == *list_head)
    {
        *list_head = new;
        return 0;
    }
    
    last = *list_head;
    while (last->next->node != NULL)
    {
        last = last->next->node;
    }

    last->next->node = new;
    new->next->node = NULL;
    new->next->operator = operator;

    return 0;
}

int ast_node_push(struct node **list_head, struct node *new)
{
    if (NULL == new)
    {
        DEBUG;
        return -1;
    }

    if (NULL == *list_head)
    {
        *list_head = new;
        return 0;
    }

    new->next->node = *list_head;
    *list_head = new;

    return 0;
}

const struct node *ast_node_index(const struct node *list_head, int index)
{
    for (int i=0; i<index; i++)
    {
        list_head = list_head->next->node;
    }

    return list_head;
}

size_t ast_num_nodes(const struct node *list_head)
{
    if (NULL == list_head)
    {
        return 0;
    }

    return 1 + ast_num_nodes(list_head->next->node);
}

void ast_print_next(const struct node *node, int level)
{
    struct node *next;

    if (NULL != node)
    {
        if (NULL != node->next)
        {
            if (NULL != node->next->node)
            {
                next = node->next->node;
                while (NULL != next)
                {
                    ast_print(next, level, "next");
                    if (NULL != node->left)
                    {
                        printf("Level %d, %s, %s: %s \n", level, "next-left", node->left->type, token_get_display(node->left->op));
                    }
                    
                    if (NULL != node->right)
                    {
                        printf("Level %d, %s, %s: %s \n", level, "next-right", node->right->type, token_get_display(node->right->op));
                    }
                    printf("\n");

                    next = next->next->node;
                }
            }
        }
    }
}

void ast_print(const struct node *ast, int level, char *location)
{
    int print_left, print_right;

    print_left = print_right = 0;
    if (NULL != ast->op)
    {
        printf("Level %d, %s, %s: %s \n", level, location, ast->type, token_get_display(ast->op));
        ast_print_next(ast, level);
        if (NULL != ast->left)
        {
            printf("Level %d, %s, %s: %s \n", level, location, ast->left->type, token_get_display(ast->left->op));
            ast_print_next(ast->left, level);
            print_left = 1;
        }
        
        if (NULL != ast->right)
        {
            printf("Level %d, %s, %s: %s \n", level, location, ast->right->type, token_get_display(ast->right->op));
            ast_print_next(ast->right, level);
            print_right = 1;
        }
        printf("\n");

        if (print_left)
        {
            ast_print(ast->left, level + 1, "left");
        }
        if (print_right)
        {
            ast_print(ast->right, level + 1, "right");
        }
    }
}
