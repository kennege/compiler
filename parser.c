#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "token.h"
#include "ast.h"
#include "lexer.h"
#include "parser.h"

struct node *term(const char *input_str, struct window *window);
struct node *expression(const char *input_str, struct window *window);

struct node *factor(const char *input_str, struct window *window)
{
    /* factor : INTEGER | LPAREN expr RPAREN */
    struct token *lparen, *token, *rparen;
    struct node *node;
    
    token = lexer_eat(INTEGER, input_str, window);
    if (NULL != token)
    {
        return ast_node_set(token);
    }  
 
    lparen = lexer_eat(LPAREN, input_str, window);
    if (NULL != lparen)
    {
        node = expression(input_str, window);
        if (NULL == node)
        {
            DEBUG;
            lparen = token_destroy(lparen);
            return ast_destroy(node);
        }
        rparen = lexer_eat(RPAREN, input_str, window);
        if (NULL == rparen)
        {
            DEBUG;
            lparen = token_destroy(lparen);
            return ast_destroy(node);
        }
    }
    rparen = token_destroy(rparen);
    lparen = token_destroy(lparen);

    return node; 
}

struct node *term(const char *input_str, struct window *window)
{   
    /* term : factor ((MULTIPLY | DIVIDE) factor) * */
    struct token *op;
    struct node *node;

    node = factor(input_str, window);
    if (NULL == node)
    {
        return NULL;
    }

    while (1)
    {
        op = lexer_eat(MULTIPLY, input_str, window);
        if (NULL != op)
        {
            node = ast_node_add(node, op, factor(input_str, window));
            continue;
        }
        
        op = lexer_eat(DIVIDE, input_str, window);
        if (NULL != op)
        {
            node = ast_node_add(node, op, factor(input_str, window));
            continue;
        }  
        break;
    }

    return node;
}

struct node *expression(const char *input_str, struct window *window)
{
    /* expr : term ((PLUS | MINUS) term) * */
    struct token *op;
    struct node *node;

    node = term(input_str, window);
    if (NULL == node)
    {
        return NULL;
    }

    while (1)
    {
        op = lexer_eat(PLUS, input_str, window);
        if (NULL != op)
        {
            node = ast_node_add(node, op, factor(input_str, window));
            continue;
        }
        
        op = lexer_eat(MINUS, input_str, window);
        if (NULL != op)
        {
            node = ast_node_add(node, op, factor(input_str, window));
            continue;
        } 
        break; 
    }

    return node;
}

struct node *parser_parse(const char *input_str)
{    
    struct window window;

    window.left = window.right = 0;

    return expression(input_str, &window);
}