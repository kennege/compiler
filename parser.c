#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "token.h"
#include "ast.h"
#include "lexer.h"
#include "parser.h"

struct node *expression(struct token **token_list);

struct node *variable(struct token **token_list)
{
    /* VAR WORD VARTYPE | VARTYPE WORD | WORD */
    struct node *node;
    struct token *token;

    if (0 == token_compare(*token_list, WORD))
    {
        token = token_list_pop(token_list);
        return ast_variable_node_add(token);
    }

    if (0 == token_compare(*token_list, VARTYPE))
    {
        token_list = token_list_step(token_list);
        if (0 != token_compare(*token_list, WORD))
        {
            DEBUG;
            return NULL;
        }
        token = token_list_pop(token_list);
        return ast_variable_node_add(token);
    }

    if (0 == token_compare(*token_list, VAR))
    {
        token_list = token_list_step(token_list);
        if (0 != token_compare(*token_list, WORD))
        {
            DEBUG;
            return NULL;
        }
        token = token_list_pop(token_list);
        node = ast_variable_node_add(token);

        if (0 != token_compare(*token_list, VARTYPE))
        {
            DEBUG;
            return NULL;
        }
        token_list = token_list_step(token_list);
    }
    
    return node;
}

struct node *factor(struct token **token_list)
{
    /* factor : (PLUS | MINUS) factor (INTEGER | LPAREN | WORD) expr RPAREN */
    struct node *node;
    struct token *token;

    if (0 == token_list_compare_any(token_list, 2, PLUS, MINUS))
    {
        token = token_list_pop(token_list);
        return ast_unary_node_add(token, factor(token_list));
    }
    
    if (0 == token_list_compare_any(token_list, 2, INTEGER, FLOAT))
    {
        return ast_value_node_set(token_list_pop(token_list));
    } 

    if (0 == token_compare(*token_list, WORD))
    {
        return variable(token_list);
    }
 
    if (0 == token_compare(*token_list, LPAREN))
    {
        node = expression(token_list_step(token_list));
        if (NULL == node)
        {
            DEBUG;
            return ast_destroy(node);
        }

        if (0 != token_compare(*token_list, RPAREN))
        {
            DEBUG;
            return ast_destroy(node);
        }
        token_list = token_list_step(token_list);
    }
    
    return node; 
}

struct node *term(struct token **token_list)
{   
    /* term : factor ((MULTIPLY | DIVIDE) factor) * */
    struct node *node;
    struct token *token;

    node = factor(token_list);
    if (NULL == node)
    {
        return NULL;
    }

    while (1)
    {
        if (0 == token_list_compare_any(token_list, 2, MULTIPLY, DIVIDE))
        {
            token = token_list_pop(token_list);
            node = ast_binary_node_add(node, token, factor(token_list));
            continue;
        }
        break;
    }

    return node;
}

struct node *expression(struct token **token_list)
{
    /* expr : term (PLUS | MINUS) term * */
    struct node *node;
    struct token *token;

    node = term(token_list);
    if (NULL == node)
    {
        return NULL;
    }

    while (1)
    {
        if (0 == token_list_compare_any(token_list, 2, MINUS, PLUS))
        {
            token = token_list_pop(token_list);
            node = ast_binary_node_add(node, token, term(token_list));
            continue;
        }
        break; 
    }

    return node;
}

struct node *assignment(struct token **token_list)
{
    /* assignment : variable EQUALS expression |
                    variable ASSIGN expression | */ 
    struct node *node;
    struct token *token;

    node = variable(token_list);
    if (NULL == node)
    {
        return NULL;
    }

    if (0 == token_list_compare_any(token_list, 2, EQUALS, ASSIGN))
    {
        token = token_list_pop(token_list);
        node = ast_assignment_node_add(node, token, expression(token_list));
        if (NULL == node)
        {
            DEBUG;
            return NULL;
        }
        return node;
    }

    return NULL;
}   

struct node *assignment_list(struct token **token_list)
{
    /* assignment list : assignment | assignment NEWLINE assignment_list */
    struct node *node;

    node = assignment(token_list);
    if (NULL == node)
    {
        return NULL;
    }

    while (1)
    {
        if (0 == token_compare(*token_list, NEWLINE_TYPE))
        {
            token_list = token_list_step(token_list);
            if (0 != ast_assignment_node_append(node, assignment(token_list)))
            {
                DEBUG;
                return NULL;
            }
            continue;
        }
        break;
    }

    return node;
}

struct node *program(struct token **token_list)
{
    /* program : FUNC WORD LBRACE assignment_list RBRACE | 
                assignment_list | expression */

    struct node *node;

    if (0 == token_compare(*token_list, FUNC))
    { //TODO: doesn't handle function name() or \n
        token_list = token_list_step(token_list);
        if (0 == token_compare(*token_list, LBRACE))
        {
            token_list = token_list_step(token_list);
            node = assignment_list(token_list);
            if (NULL == node)
            {
                DEBUG;
                return NULL;
            }

            if (0 != token_compare(*token_list, RBRACE))
            {
                DEBUG;
                return ast_destroy(node);
            }
            token_list = token_list_step(token_list);
        }
        return node;
    }

    node = assignment_list(token_list);
    if (NULL != node)
    {
        return node;
    }

    return expression(token_list);
}

struct node *parser_parse(struct token *token_list)
{    
    return program(&token_list);
}