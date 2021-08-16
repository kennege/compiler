#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "token.h"
#include "ast.h"
#include "lexer.h"
#include "parser.h"

struct node *expression(struct token **token_list);

struct node *factor(struct token **token_list)
{
    /* factor : (PLUS | MINUS) factor : INTEGER | LPAREN expr RPAREN */
    struct token *lparen, *token, *rparen;
    struct node *node;

    token = lexer_eat(PLUS, token_list);
    if (NULL != token)
    {
        return ast_unary_node_add(token, factor(token_list));
    }
    
    token = lexer_eat(MINUS, token_list);
    if (NULL != token)
    {
        return ast_unary_node_add(token, factor(token_list));
    }

    token = lexer_eat(INTEGER, token_list);
    if (NULL != token)
    {
        return ast_value_node_set(token);
    }  
 
    lparen = lexer_eat(LPAREN, token_list);
    if (NULL != lparen)
    {
        node = expression(token_list);
        if (NULL == node)
        {
            DEBUG;
            return ast_destroy(node);
        }
        rparen = lexer_eat(RPAREN, token_list);
        if (NULL == rparen)
        {
            DEBUG;
            return ast_destroy(node);
        }
    }
    
    return node; 
}

struct node *term(struct token **token_list)
{   
    /* term : factor ((MULTIPLY | DIVIDE) factor) * */
    struct token *op;
    struct node *node;

    node = factor(token_list);
    if (NULL == node)
    {
        return NULL;
    }

    while (1)
    {
        op = lexer_eat(MULTIPLY, token_list);
        if (NULL != op)
        {
            node = ast_binary_node_add(node, op, factor(token_list));
            continue;
        }
        
        op = lexer_eat(DIVIDE, token_list);
        if (NULL != op)
        {
            node = ast_binary_node_add(node, op, factor(token_list));
            continue;
        }  
        break;
    }

    return node;
}

struct node *expression(struct token **token_list)
{
    /* expr : term ((PLUS | MINUS) term) * */
    struct token *op;
    struct node *node;

    node = term(token_list);
    if (NULL == node)
    {
        return NULL;
    }

    while (1)
    {
        op = lexer_eat(PLUS, token_list);
        if (NULL != op)
        {
            node = ast_binary_node_add(node, op, factor(token_list));
            continue;
        }
        
        op = lexer_eat(MINUS, token_list);
        if (NULL != op)
        {
            node = ast_binary_node_add(node, op, factor(token_list));
            continue;
        } 
        break; 
    }

    return node;
}

struct node *parser_parse(struct token *token_list)
{    
    return expression(&token_list);
}