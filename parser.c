#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "token.h"
#include "ast.h"
#include "lexer.h"
#include "parser.h"

struct node *parser_parse_expression(struct token **token_list);

struct node *parser_parse_factor(struct token **token_list)
{
    /* factor : [NEWLINE | SEMICOLON] (PLUS | MINUS | STRING) factor (NUMBER | (LPAREN expr RPAREN)) */
    struct node *node;
    struct token *token;

    if (0 == token_list_compare_any(token_list, 2, NEWLINE_TYPE, SEMICOLON))
    {   
        token_list = token_list_step(token_list);
        return parser_parse_factor(token_list);
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
        token = token_list_pop(token_list);
        return ast_variable_node_add(token);
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

        return node;
    }
    
    return NULL; 
}

struct node *parser_parse_term(struct token **token_list)
{   
    /* term : factor ((MULTIPLY | DIVIDE) factor) * */
    struct node *node;
    struct token *token;

    node = parser_parse_factor(token_list);
    if (NULL == node)
    {
        DEBUG;
        token_list_print(*token_list);
        return NULL;
    }

    while (1)
    {
        if (0 == token_list_compare_any(token_list, 2, MULTIPLY, DIVIDE))
        {
            token = token_list_pop(token_list);
            node = ast_binary_node_add(node, token, parser_parse_factor(token_list));
            if (NULL == node)
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
            if (NULL == node)
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

struct node *parser_parse_assignment_or_declaration(struct token **token_list)
{
    /* declaration : (VAR STRING VARTYPE EQUALS expression) |
                    (STRING ASSIGN expression) */
    /* assignment  : (STRING EQUALS expression) */ 

    struct node *node;
    struct token *name;

    if (0 == token_compare(*token_list, VAR))
    {
        token_list = token_list_step(token_list);
        if (0 != token_compare(*token_list, STRING))
        {
            DEBUG;
            return NULL;
        }
        name = token_list_pop(token_list);
        node = ast_variable_node_add(name);
        if (NULL == node)
        {
            DEBUG;
            return NULL;
        }

        if (0 != token_compare(*token_list, VARTYPE))
        {
            DEBUG;
            return ast_destroy(node);
        }
        token_list = token_list_step(token_list);
        if (0 != token_compare(*token_list, EQUALS))
        {
            DEBUG;
            return ast_destroy(node);
        }
        token_list = token_list_step(token_list);
        return ast_declaration_node_add(node, name, parser_parse_expression(token_list));
    }

    if (0 == token_compare(*token_list, STRING))
    {
        name = token_list_pop(token_list);
        node = ast_variable_node_add(name);
        if (0 == token_compare(*token_list, ASSIGN))
        {
            token_list = token_list_step(token_list);
            return ast_declaration_node_add(node, name, parser_parse_expression(token_list));
        }
        if (0 == token_compare(*token_list, EQUALS))
        {
            token_list = token_list_step(token_list);
            return ast_assignment_node_add(node, name, parser_parse_expression(token_list));     
        }
        DEBUG;
        return NULL;       
    }

    return NULL;
}   

struct node *parser_parse_assignment_or_declaration_list(struct token **token_list)
{
    /* assignment list : assignment | (assignment (NEWLINE | SEMICOLON) assignment_list) */
    /* declaration list : declaration | (declaration (NEWLINE | SEMICOLON) declaration_list) */
    struct node *node;

    node = parser_parse_assignment_or_declaration(token_list);
    if (NULL == node)
    {
        return NULL;
    }

    while (1)
    {
        if (0 == token_list_compare_any(token_list, 2, NEWLINE_TYPE, SEMICOLON))
        {
            token_list = token_list_step(token_list);
            if (0 == ast_node_append(node, parser_parse_assignment_or_declaration(token_list)))
            {
                continue;
            }         
        }
        break;
    }

    return node;
}

// struct token *parser_parse_function_arguments(struct token **token_list)
// {

// }

struct token *parser_parse_function_declaration(struct token **token_list)
{
    /* function declaration : FUNC STRING LPAREN [arguments] [VARTYPE] RPAREN 
        [NEWLINE | SEMICOLON] [RETURN (STRING | INT | FLOAT )] [NEWLINE | SEMICOLON] LBRACE */
    struct token *name;

    if (0 != token_list_compare_all(token_list, 4, FUNC, STRING, LPAREN, RPAREN))
    {   
        return NULL;
    }
    token_list = token_list_step(token_list);
    name = token_list_pop(token_list);
    token_list = token_list_step(token_list_step(token_list));
    
    while (0 == token_list_compare_any(token_list, 2, NEWLINE_TYPE, SEMICOLON))
    {
        token_list = token_list_step(token_list);
    }

    if (0 == token_compare(*token_list, VARTYPE))
    {
        token_list = token_list_step(token_list); 
    }

    if (0 != token_compare(*token_list, LBRACE))
    {
        DEBUG;
        return NULL;
    }
    token_list = token_list_step(token_list);

    return name;
}

struct node *parser_parse_function(struct token **token_list)
{
    /*  function : function declaration [NEWLINE | SEMICOLON] assignment_list
     [NEWLINE | SEMICOLON] [RETURN] [NEWLINE | SEMICOLON] RBRACE */
    struct node *node, *return_node;
    struct token *name;

    name = parser_parse_function_declaration(token_list);
    if (NULL == name)
    {
        return NULL;
    }

    while(1)
    {
        if (0 == token_list_compare_any(token_list, 2, NEWLINE_TYPE, SEMICOLON))
        {
            token_list = token_list_step(token_list);
            node = ast_function_node_add(name, parser_parse_assignment_or_declaration_list(token_list));
            if (NULL == node)
            {
                DEBUG;
                return NULL;
            }
            continue;
        }
        break;
    }

    if (0 == token_compare(*token_list, RETURN))
    {
        token_list = token_list_step(token_list);
        return_node = parser_parse_factor(token_list);
        if (NULL == return_node)
        {
            DEBUG;
            return ast_destroy(node);
        }
        if ( 0 != ast_function_node_add_return(node, return_node))
        {
            DEBUG;
            return ast_destroy(return_node);
            return ast_destroy(node);
        }
    }

    while (0 == token_list_compare_any(token_list, 2, NEWLINE_TYPE, SEMICOLON))
    {
        token_list = token_list_step(token_list);
    }

    if (0 != token_compare(*token_list, RBRACE))
    {
        DEBUG;
        return ast_destroy(node);
    }
    token_list = token_list_step(token_list);

    return node;
}

struct node *parser_parse_function_list(struct token **token_list)
{
    /* function list : function | (function (NEWLINE | SEMICOLON) function_list) */
    struct node *node;

    node = parser_parse_function(token_list);
    if (NULL == node)
    {
        return NULL;
    }

    while (1)
    {
        if (0 == token_list_compare_any(token_list, 2, NEWLINE_TYPE, SEMICOLON))
        {
            token_list = token_list_step(token_list);
            if (0 == ast_node_append(node, parser_parse_function(token_list)))
            {
                continue;
            }
        }
        break;
    }

    return node;
}

struct node *parser_parse_program(struct token **token_list)
{ //TODO: remove options that dont involve functions
    /* program : function_list | assignment_list | expression */

    struct node *node;

    node = parser_parse_function_list(token_list);
    if (NULL != node)
    {
        return node;
    }

    node = parser_parse_assignment_or_declaration_list(token_list);
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