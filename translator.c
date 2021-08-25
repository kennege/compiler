#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "token.h"
#include "lexer.h"
#include "ast.h"
#include "stack.h"
#include "integer.h"
#include "float.h"
#include "string.h"
#include "translator.h"

struct stack *stack = NULL;

struct token *translator_visit(const struct node *node);

static struct token *translator_operate_unary(const struct token *token, const char *op)
{
    char *type;
    struct token *result;

    struct {
        char *type;
        struct token *(*operations)(const struct token *token, const char *op);
    } op_types[] = {
        { .type = INT, .operations = integer_unary_operations },
        { .type = FLOAT, .operations = float_unary_operations },
    };

    type = token_get_type(token);
    if (NULL == type)
    {
        return NULL;
    }
    
    for (int i=0; i<LENGTH(op_types); i++)
    {
        if (0 == strcmp(type, op_types[i].type))
        {
            result = op_types[i].operations(token, op);
            if (NULL != result)
            {
                return result;
            }
            fprintf(stderr, "ERROR. Operation %s could not be performed on type %s\n", op, type);
            return NULL;
        }
    }

    DEBUG;
    return NULL;
}

static struct token *translator_operate_binary(const struct token *left, const struct token *right, const char *op)
{
    char *type;
    struct token *result;

    struct {
        char *type;
        struct token *(*operations)(const struct token *left, const struct token *right, const char *op);
    } op_types[] = {
        { .type = INT, .operations = integer_binary_operations },
        { .type = FLOAT, .operations = float_binary_operations },
        { .type = STRING, .operations = string_binary_operations },
    };

    if (0 != strcmp(token_get_type(left), token_get_type(right)))
    {
        fprintf(stderr, "ERROR: operation between different types.\n");
        return NULL;
    }
    type = token_get_type(left);
    
    for (int i=0; i<LENGTH(op_types); i++)
    {
        if (0 == strcmp(type, op_types[i].type))
        {
            result = op_types[i].operations(left, right, op);
            if (NULL != result)
            {
                return result;
            }
            fprintf(stderr, "ERROR. Operation %s could not be performed on type %s\n", op, type);
            return NULL;
        }
    }

    DEBUG;
    return NULL;
}

static struct token *translator_visit_assignment_node(const struct node *node)
{
    const struct node *next_node;
    size_t n_assignment_nodes;

    n_assignment_nodes = ast_assignment_node_length(node);
    for (int i=0; i<n_assignment_nodes; i++)
    {
        next_node = ast_assignment_node_index(node, i);
        if (0 != stack_push(&stack, next_node->left->op, translator_visit(next_node->right)))
        {
            DEBUG;
            return NULL;
        }
    }
    
    DEBUG;
    return NULL;
}

static struct token *translator_visit_variable_node(const struct node *node)
{
    struct token *output;

    output = stack_extract(stack, node->op);
    if (NULL == output)
    {
        fprintf(stderr, "ERROR: variable not found\n");
        return NULL;
    }

    return output;
}

static struct token *translator_visit_binary_node(const struct node *node)
{
    struct token *left, *right;

    struct {
        char *op;
    } binary_ops[] = {
        { .op = PLUS },
        { .op = MINUS },
        { .op = MULTIPLY },
        { .op = DIVIDE },
    };

    for (int i=0; i<LENGTH(binary_ops); i++)
    {
        if (0 == token_compare(node->op, binary_ops[i].op))
        {    
            left = translator_visit(node->left);
            right = translator_visit(node->right);
            if (NULL == left || NULL == right)
            {
                DEBUG;
                return NULL;
            }
            return translator_operate_binary(left, right, binary_ops[i].op); 
        }
    }

    DEBUG;
    return NULL;
}

static struct token *translator_visit_unary_node(const struct node *node)
{  
    struct token *output;

    struct {
        char *op;
    } unary_ops[] = {
        { .op = PLUS },
        { .op = MINUS },
    };
    
    for (int i=0; i<LENGTH(unary_ops); i++)
    {
        if (0 == token_compare(node->op, unary_ops[i].op))
        {
            output = translator_visit(node->left);
            return translator_operate_unary(output, unary_ops[i].op);
        }
    }

    DEBUG;
    return NULL;
}

static struct token *translator_visit_value_node(const struct node *node)
{
    return translator_operate_unary(node->op, PLUS);
}

struct token *translator_visit(const struct node *node)
{
    static const struct {
        char *node_type;
        struct token *(*fn)(const struct node *node);
    } tab[] = {
        { .node_type = BINARY, .fn = translator_visit_binary_node },
        { .node_type = UNARY, .fn = translator_visit_unary_node },
        { .node_type = VALUE, .fn = translator_visit_value_node },
        { .node_type = VARIABLE, .fn = translator_visit_variable_node },
        { .node_type = ASSIGNMENT, .fn = translator_visit_assignment_node },
    };

    for (int i=0; i<LENGTH(tab); i++)
    {
        if (0 == strcmp(node->type, tab[i].node_type))
        {
            return tab[i].fn(node);
        }
    }

    DEBUG;
    return NULL;
}

struct token *translator_translate(const struct node *ast)
{
    struct token *result;

    if (NULL == ast)
    {
        DEBUG;
        return NULL;
    }

    ast_print(ast, 0, "root");
    result = translator_visit(ast);
    stack_print(stack);

    stack = stack_destroy_all(stack);
    
    return result;
}
