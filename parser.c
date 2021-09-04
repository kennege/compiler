#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "token.h"
#include "ast.h"
#include "lexer.h"
#include "parser.h"

/* GRAMMAR

    factor : [NEWLINE | SEMICOLON] (PLUS | MINUS | STRING) factor (NUMBER | (LPAREN expr RPAREN))
    
    term : factor ((MULTIPLY | DIVIDE) factor) * 
    
    expression : term (PLUS | MINUS) term * 
    
    declaration_without_value : VAR STRING VARTYPE
    
    declaration_with_value : declaration_without_value EQUALS expression |
        STRING ASSIGN expression

    assignment : STRING EQUALS expression

    statement_list : statement | (statement (NEWLINE | SEMICOLON) statement_list)

    function arguments : VARTYPE STRING COMMA *

    function declaration : FUNC STRING LPAREN [function_arguments] RPAREN [VARTYPE] 
        [NEWLINE | SEMICOLON] [RETURN (STRING | INT | FLOAT )] [NEWLINE | SEMICOLON] LBRACE

    function : function declaration [NEWLINE | SEMICOLON] assignment_list
        [NEWLINE | SEMICOLON] [RETURN] [NEWLINE | SEMICOLON] RBRACE

    function list : function | (function [NEWLINE | SEMICOLON] function_list)

    program : function_list | assignment_list | expression

*/

struct node *parser_parse_expression(struct token **token_list);

struct node *parser_parse_factor(struct token **token_list)
{
    /* factor : [NEWLINE | SEMICOLON] (PLUS | MINUS | STRING) factor (NUMBER | (LPAREN expr RPAREN)) */
    struct node *node;
    struct token *token;

    node = NULL;
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
        token = token_list_pop(token_list);
        return ast_value_node_set(token);
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
    
    return node; 
}

struct node *parser_parse_term(struct token **token_list)
{   
    /* term : factor ((MULTIPLY | DIVIDE) factor) * */
    struct node *node;
    struct token *op;

    node = parser_parse_factor(token_list);
    if (NULL == node)
    {
        DEBUG;
        return NULL;
    }

    while (1)
    {
        if (0 == token_list_compare_any(token_list, 2, MULTIPLY, DIVIDE))
        {
            op = token_list_pop(token_list);
            node = ast_binary_node_add(node, op, parser_parse_factor(token_list));
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
    /* expression : term (PLUS | MINUS) term * */
    struct node *node;
    struct token *op;

    node = parser_parse_term(token_list);
    if (NULL == node)
    {
        return NULL;
    }
    while (1)
    {
        if (0 == token_list_compare_any(token_list, 2, MINUS, PLUS))
        {
            op = token_list_pop(token_list);
            node = ast_binary_node_add(node, op, parser_parse_term(token_list));
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

struct node *parser_parse_declaration_without_value(struct token **token_list)
{
    /* declaration_without_value : VAR STRING VARTYPE */
    struct node *node;
    struct token *name; 

    node = NULL;
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
    }

    return node;
}

struct node *parser_parse_declaration_with_value(struct token **token_list)
{
    /* declaration_with_value : VAR STRING VARTYPE EQUALS expression  */
    struct node *node;
    struct token *name, *op;   

    node = NULL;
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
        op = token_list_pop(token_list);
        return ast_declaration_node_add(node, op, parser_parse_expression(token_list));
    }

    return node;
}

struct node *parser_parse_assignment(struct token **token_list)
{
    /* assignment : (STRING EQUALS expression) | (STRING ASSIGN expression)*/ 
    struct node *node;
    struct token *name, *op;

    node = NULL;
    if (0 == token_compare(*token_list, STRING))
    {
        name = token_list_pop(token_list);
        node = ast_variable_node_add(name);
        if (0 == token_compare(*token_list, EQUALS))
        {
            op = token_list_pop(token_list);
            return ast_assignment_node_add(node, op, parser_parse_expression(token_list));     
        }
        if (0 == token_compare(*token_list, ASSIGN))
        {
            op = token_list_pop(token_list);
            return ast_declaration_node_add(node, op, parser_parse_expression(token_list));
        }
        DEBUG;     
    }

    return node;
}

struct node *parser_parse_statement(struct token **token_list)
{
    /* declaration_with_value | declaration_without_value | assignment */
    struct node *node;
    
    while (0 == token_list_compare_any(token_list, 2, NEWLINE_TYPE, SEMICOLON))
    {
        token_list = token_list_step(token_list);
    }

    node = parser_parse_declaration_with_value(token_list);
    if (NULL != node)
    {
        return node;
    }

    node = parser_parse_declaration_without_value(token_list);
    if (NULL != node)
    {
        return node;
    }

    node = parser_parse_assignment(token_list);
    if (NULL != node)
    {
        return node;
    }   

    return NULL;
}   

struct node *parser_parse_statement_list(struct token **token_list)
{
    /* statement_list : statement | (statement (NEWLINE | SEMICOLON) statement_list) */ 
    struct node *node, *next;

    node = parser_parse_statement(token_list);
    if (NULL == node)
    {
        return NULL;
    }

    while (1)
    {
        if (0 == token_list_compare_any(token_list, 2, NEWLINE_TYPE, SEMICOLON))
        {
            token_list = token_list_step(token_list);
            next = parser_parse_statement(token_list);
            if (NULL == next)
            {
                return node;
            }
            if (0 == ast_node_append(&node, next))
            {
                continue;
            }  
            else
            {
                DEBUG;
            }       
        }
        break;
    }

    return node;
}

struct node *parser_parse_function_arguments(struct token **token_list)
{
    /* function arguments : declaration_without_value COMMA * */
    struct node *node, *next;

    node = parser_parse_declaration_without_value(token_list);
    if (NULL == node)
    {
        return NULL;
    }

    while (1)
    {
        if (0 == token_compare(*token_list, COMMA))
        {
            token_list = token_list_step(token_list);
            next = parser_parse_declaration_without_value(token_list);
            if (NULL == next)
            {
                return node;
            }
            if (0 == ast_node_append(&node, next))
            {
                continue;
            }
        }
        break;
    }

    return node;
}

struct node *parser_parse_function_declaration(struct token **token_list)
{
    /* function declaration : FUNC STRING LPAREN [function_arguments] RPAREN [VARTYPE]  
        [NEWLINE | SEMICOLON] LBRACE */
    struct token *name;
    struct node *node, *arguments;

    node = NULL;
    if (0 != token_list_compare_all(token_list, 3, FUNC, STRING, LPAREN))
    {   
        return NULL;
    }

    token_list = token_list_step(token_list);
    name = token_list_pop(token_list);
    
    node = ast_function_node_add(name);
    if (NULL == node)
    {
        DEBUG;
        return NULL;
    }
    token_list = token_list_step(token_list);

    while (0 != token_compare(*token_list, RPAREN))
    {
        token_list = token_list_step(token_list);
        arguments = parser_parse_function_arguments(token_list);
        if (NULL != arguments)
        {
            if (0 != ast_function_node_add_arguments(node, arguments))
            {
                DEBUG;
                arguments = ast_destroy(arguments);
                return ast_destroy(node);
            }
        }
    }
    token_list = token_list_step(token_list);
    
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
        return ast_destroy(node);
    }
    token_list = token_list_step(token_list);

    return node;
}

struct node *parser_parse_function(struct token **token_list)
{
    /*  function : function declaration [NEWLINE | SEMICOLON] assignment_list
     [NEWLINE | SEMICOLON] [RETURN] [NEWLINE | SEMICOLON] RBRACE */
    struct node *node, *return_node;

    node = parser_parse_function_declaration(token_list);
    if (NULL == node)
    {
        return NULL;
    }

    if (0 == token_list_compare_any(token_list, 2, NEWLINE_TYPE, SEMICOLON))
    {
        token_list = token_list_step(token_list);
        if (0 != ast_function_node_add_body(node, parser_parse_statement_list(token_list)))
        {
            DEBUG;
            return ast_destroy(node);
        }
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
    /* function list : function | (function [NEWLINE | SEMICOLON] function_list) */
    struct node *node, *next;

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
            next = parser_parse_function(token_list);
            if (NULL == next)
            {
                return node;
            }
            if (0 == ast_node_append(&node, next))
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

    node = parser_parse_statement_list(token_list);
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