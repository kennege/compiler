#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "token.h"
#include "lexer.h"
#include "ast.h"
#include "memory.h"
#include "translator.h"

struct kv *memory_space = NULL;

/* currently interprets, will eventually translate to assembly */
int translator_visit(const struct node *node, FILE *fp);

static int translator_visit_assignment_node(const struct node *node, FILE *fp)
{
    const struct node *next_node;
    size_t n_assignment_nodes;

    n_assignment_nodes = ast_assignment_node_length(node);
    for (int i=0; i<n_assignment_nodes; i++)
    {
        next_node = ast_assignment_node_index(node, i);
        if (0 != memory_append(&memory_space, next_node->left->id, translator_visit(next_node->right, fp)))
        {
            DEBUG;
            return 0;
        }
    }

    return translator_visit(next_node->right, fp);
}

static int translator_visit_variable_node(const struct node *node, FILE *fp)
{
    return memory_extract(memory_space, node->id);
}

static int translator_visit_binary_node(const struct node *node, FILE *fp)
{
    if (0 == token_compare(node->op, PLUS))
    {
        return translator_visit(node->left, fp) + translator_visit(node->right, fp);
    }

    if (0 == token_compare(node->op, MINUS))
    {
        return translator_visit(node->left, fp) - translator_visit(node->right, fp);
    }

    if (0 == token_compare(node->op, MULTIPLY))
    {
        return translator_visit(node->left, fp) * translator_visit(node->right, fp);
    }

    if (0 == token_compare(node->op, DIVIDE))
    {
        return translator_visit(node->left, fp) / translator_visit(node->right, fp);
    }

    DEBUG;
    return 0;
}

static int translator_visit_unary_node(const struct node *node, FILE *fp)
{
    if (0 == token_compare(node->op, PLUS))
    {
        return translator_visit(node->left, fp);
    }

    if (0 == token_compare(node->op, MINUS))
    {
        return - translator_visit(node->left, fp);
    }

    DEBUG;
    return 0;
}

static int translator_visit_value_node(const struct node *node, FILE *fp)
{
    char *q;

    return strtol(token_get_value(node->op), &q, 10);
}

int translator_visit(const struct node *node, FILE *fp)
{
    static const struct {
        char *node_type;
        int (*fn)(const struct node *node, FILE *fp);
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
            return tab[i].fn(node, fp);
        }
    }

    DEBUG;
    return 0;
}

int translator_translate(const struct node *ast, FILE *fp)
{
    int result;

    result = translator_visit(ast, fp);

    // memory_print(memory_space);

    memory_space = memory_destroy_all(memory_space);
    
    return result;
}
