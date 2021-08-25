#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "token.h"
#include "ast.h"
#include "lexer.h"
#include "parser.h"

struct node *parser_parse_expression(struct token **token_list);

struct node *parser_parse_variable(struct token **token_list)
{
    /* VAR STRING VARTYPE | VARTYPE STRING | STRING */
    struct node *node;
    struct token *token;

    if (0 == token_compare(*token_list, STRING))
    {
        token = token_list_pop(token_list);
        return ast_variable_node_add(token);
    }

    if (0 == token_compare(*token_list, VARTYPE))
    {
        token_list = token_list_step(token_list);
        if (0 != token_compare(*token_list, STRING))
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
        if (0 != token_compare(*token_list, STRING))
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

struct node *parser_parse_factor(struct token **token_list)
{
    /* factor : (PLUS | MINUS | STRING) factor (NUMBER | LPAREN expr RPAREN) */
    struct node *node;
    struct token *token;

    if (0 == token_compare(*token_list, NEWLINE_TYPE))
    {   
        return parser_parse_factor(token_list_step(token_list));
    }

    if (0 == token_list_compare_any(token_list, 2, PLUS, MINUS))
    {
        token = token_list_pop(token_list);
        return ast_unary_node_add(token, parser_parse_factor(token_list));
    }
    
    if (0 == token_list_compare_any(token_list, 2, INT, FLOAT))
    {
        return ast_value_node_set(token_list_pop(token_list));
    } 

    if (0 == token_compare(*token_list, STRING))
    {
        return parser_parse_variable(token_list);
    }
 
    if (0 == token_compare(*token_list, LPAREN))
    {
        node = parser_parse_expression(token_list_step(token_list));
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

struct node *parser_parse_term(struct token **token_list)
{   
    /* term : factor ((MULTIPLY | DIVIDE) factor) * */
    struct node *node;
    struct token *token;

    node = parser_parse_factor(token_list);
    if (NULL == node)
    {
        return NULL;
    }

    while (1)
    {
        if (0 == token_list_compare_any(token_list, 2, MULTIPLY, DIVIDE))
        {
            token = token_list_pop(token_list);
            node = ast_binary_node_add(node, token, parser_parse_factor(token_list));
            continue;
        }
        break;
    }

    return node;
}

struct node *parser_parse_expression(struct token **token_list)
{
    /* expr : term (PLUS | MINUS) term * */
    struct node *node;
    struct token *token;

    node = parser_parse_term(token_list);
    if (NULL == node)
    {
        return NULL;
    }

    while (1)
    {
        if (0 == token_list_compare_any(token_list, 2, MINUS, PLUS))
        {
            token = token_list_pop(token_list);
            node = ast_binary_node_add(node, token, parser_parse_term(token_list));
            continue;
        }
        break; 
    }

    return node;
}

struct node *parser_parse_assignment(struct token **token_list)
{
    /* assignment : variable EQUALS expression |
                    variable ASSIGN expression | */ 
    struct node *node;
    struct token *token;

    node = parser_parse_variable(token_list);
    if (NULL == node)
    {
        return NULL;
    }

    if (0 == token_list_compare_any(token_list, 2, EQUALS, ASSIGN))
    {
        token = token_list_pop(token_list);
        node = ast_assignment_node_add(node, token, parser_parse_expression(token_list));
        if (NULL == node)
        {
            DEBUG;
            return NULL;
        }
        return node;
    }

    return NULL;
}   

struct node *parser_parse_assignment_list(struct token **token_list)
{
    /* assignment list : assignment | assignment NEWLINE assignment_list */
    struct node *node;

    node = parser_parse_assignment(token_list);
    if (NULL == node)
    {
        return NULL;
    }

    while (1)
    {
        if (0 == token_compare(*token_list, NEWLINE_TYPE))
        {
            token_list = token_list_step(token_list);
            if (0 != ast_assignment_node_append(node, parser_parse_assignment(token_list)))
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

struct node *parser_parse_program(struct token **token_list)
{
    /* program : FUNC STRING LBRACE assignment_list RBRACE | 
                assignment_list | expression */

    struct node *node;

    if (0 == token_compare(*token_list, FUNC))
    { //TODO: doesn't handle function name() or \n
        token_list = token_list_step(token_list);
        if (0 == token_compare(*token_list, LBRACE))
        {
            token_list = token_list_step(token_list);
            node = parser_parse_assignment_list(token_list);
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

    node = parser_parse_assignment_list(token_list);
    if (NULL != node)
    {
        return node;
    }

    return parser_parse_expression(token_list);
}

struct node *parser_parse(struct token *token_list)
{    
    if (NULL == token_list || 0 == token_compare(token_list, EOFILE_TYPE))
    {
        DEBUG;
        return NULL;
    }
 
    return parser_parse_program(&token_list);
}