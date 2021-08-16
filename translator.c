#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "token.h"
#include "lexer.h"
#include "ast.h"

/* currently interprets, will eventually translate to assembly */
int translator_visit(const struct node *node);

static int translator_visit_binary_node(const struct node *node)
{
    if (0 == token_type_compare(node->op, PLUS))
    {
        return translator_visit(node->left) + translator_visit(node->right);
    }

    if (0 == token_type_compare(node->op, MINUS))
    {
        return translator_visit(node->left) - translator_visit(node->right);
    }

    if (0 == token_type_compare(node->op, MULTIPLY))
    {
        return translator_visit(node->left) * translator_visit(node->right);
    }

    if (0 == token_type_compare(node->op, DIVIDE))
    {
        return translator_visit(node->left) / translator_visit(node->right);
    }

    DEBUG;
    return 0;
}

static int translator_visit_unary_node(const struct node *node)
{
    if (0 == token_type_compare(node->op, PLUS))
    {
        return translator_visit(node->left);
    }

    if (0 == token_type_compare(node->op, MINUS))
    {
        return - translator_visit(node->left);
    }

    DEBUG;
    return 0;
}

static int translator_visit_value_node(const struct node *node)
{
    char *q;

    return strtol(token_get_value(node->op), &q, 10);
}

int translator_visit(const struct node *node)
{
    static const struct {
        char *node_type;
        int (*fn)(const struct node *node);
    } tab[] = {
        { .node_type = BINARY, .fn = translator_visit_binary_node },
        { .node_type = UNARY, .fn = translator_visit_unary_node },
        { .node_type = VALUE, .fn = translator_visit_value_node },
    };

    for (int i=0; i<LENGTH(tab); i++)
    {
        if (0 == string_compare(node->type, tab[i].node_type))
        {
            return tab[i].fn(node);
        }
    }

    DEBUG;
    return 0;
}

int translator_translate(const struct node *ast)
{
    return translator_visit(ast);   
}
