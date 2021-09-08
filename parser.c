#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "token.h"
#include "ast.h"
#include "lexer.h"
#include "parser.h"

struct node *parser_parse_factor(struct token **token_list);
struct node *parser_parse_expression(struct token **token_list);
struct node *parser_parse_statement_list(struct token **token_list);

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

static int skip_newlines(struct token **token_list)
{
    int newline_found;

    newline_found = 0;
    while (1)
    {
        if (0 == token_list_compare_any(token_list, 2, NEWLINE_TYPE, SEMICOLON))
        {
            newline_found = 1;
            token_list_step(token_list);
            continue;
        }
        break;
    }

    return newline_found;
}

struct node *parser_parse_function_call(struct token **token_list)
{
    /* function_call : STRING LPAREN [factor | factor COMMA factor_list] RPAREN */
    struct node *node, *param, *params;
    struct token *name, *tmp_list;

    node = NULL;
    tmp_list = *token_list;
    if (0 == token_list_compare_all(&tmp_list, 2, STRING, LPAREN))
    {
        name = token_list_pop(&tmp_list);
        token_list_step(&tmp_list);
        node = ast_function_call_node_create(name);
        params = parser_parse_factor(&tmp_list);

        while (0 == token_compare(tmp_list, COMMA))
        {
            token_list_step(&tmp_list);
            param = parser_parse_factor(&tmp_list);
            if (NULL == params)
            {
                break;
            }
            if (0 == ast_node_append(&params, param))
            {
                continue;
            }
            else
            {
                DEBUG;
                params = ast_destroy(params);
                node = ast_destroy(node);
                return ast_destroy(param);
            }
        }
    
        if (0 != token_compare(tmp_list, RPAREN))
        {
            DEBUG;
            params = ast_destroy(params);
            node = ast_destroy(node);
            return ast_destroy(param);            
        }

        token_list_step(&tmp_list);
        ast_function_call_node_add_variables(node, params);
        *token_list = tmp_list;
    }

    return node;
}

struct node *parser_parse_factor(struct token **token_list)
{
    /* factor : STRING | ([NEWLINE | SEMICOLON] (PLUS | MINUS) (NUMBER | function_call | (LPAREN expr RPAREN))) */
    struct node *node, *function_call;
    struct token *token;

    node = NULL;
    if (0 == token_list_compare_any(token_list, 2, NEWLINE_TYPE, SEMICOLON))
    {   
        token_list_step(token_list);
        return parser_parse_factor(token_list);
    }

    if (0 == token_list_compare_any(token_list, 2, PLUS, MINUS))
    {
        token = token_list_pop(token_list);
        return ast_unary_node_create(token, parser_parse_factor(token_list));
    }
    
    if (0 == token_list_compare_any(token_list, 2, INT, FLOAT))
    {
        token = token_list_pop(token_list);
        return ast_value_node_set(token);
    } 

    function_call = parser_parse_function_call(token_list);
    if (NULL != function_call)
    {
        return function_call;
    }

    if (0 == token_compare(*token_list, STRING))
    {
        token = token_list_pop(token_list);
        return ast_variable_node_create(token);
    }
 
    if (0 == token_compare(*token_list, LPAREN))
    {
        token_list_step(token_list);
        node = parser_parse_expression(token_list);
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
        token_list_step(token_list);

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
            node = ast_binary_node_create(node, op, parser_parse_factor(token_list));
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
            node = ast_binary_node_create(node, op, parser_parse_term(token_list));
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
    struct token *name, *tmp_list; 

    node = NULL;
    tmp_list = *token_list;
    if (0 == token_compare(tmp_list, VAR))
    {
        token_list_step(&tmp_list);
        if (0 != token_compare(tmp_list, STRING))
        {
            DEBUG;
            return NULL;
        }
        name = token_list_pop(&tmp_list);
        node = ast_variable_node_create(name);
        if (NULL == node)
        {
            DEBUG;
            return NULL;
        }

        if (0 != token_compare(tmp_list, VARTYPE))
        {
            DEBUG;
            return ast_destroy(node);
        }
        
        token_list_step(&tmp_list);
        *token_list = tmp_list;
    } 

    return node;
}

struct node *parser_parse_declaration_with_value(struct token **token_list)
{
    /* declaration_with_value : VAR STRING VARTYPE EQUALS expression  */
    struct node *node;
    struct token *name, *op, *tmp_list;   

    node = NULL;
    tmp_list = *token_list;
    if (0 == token_compare(tmp_list, VAR))
    {
        token_list_step(&tmp_list);
        if (0 != token_compare(tmp_list, STRING))
        {
            DEBUG;
            return NULL;
        }
        
        name = token_list_pop(&tmp_list);      
        node = ast_variable_node_create(name);
        if (NULL == node)
        {
            DEBUG;
            return NULL;
        }

        if (0 != token_compare(tmp_list, VARTYPE))
        {
            DEBUG;
            return ast_destroy(node);
        }
        token_list_step(&tmp_list);
        
        if (0 != token_compare(tmp_list, EQUALS))
        {
            DEBUG;
            return ast_destroy(node);
        }
        
        op = token_list_pop(&tmp_list);
        *token_list = tmp_list;
        return ast_declaration_node_create(node, op, parser_parse_expression(token_list));
    }

    return node;
}

struct node *parser_parse_assignment(struct token **token_list)
{
    /* assignment : (STRING EQUALS expression) | (STRING ASSIGN expression)*/ 
    struct node *node;
    struct token *name, *op, *tmp_list;

    node = NULL;
    tmp_list = *token_list;
    if (0 == token_compare(*&tmp_list, STRING))
    {
        name = token_list_pop(&tmp_list);
        node = ast_variable_node_create(name);
        if (0 == token_compare(*&tmp_list, EQUALS))
        {
            op = token_list_pop(&tmp_list);
            *token_list = tmp_list;
            return ast_assignment_node_create(node, op, parser_parse_expression(token_list));     
        }
        if (0 == token_compare(*&tmp_list, ASSIGN))
        {
            op = token_list_pop(&tmp_list);
            *token_list = tmp_list;
            return ast_declaration_node_create(node, op, parser_parse_expression(token_list));
        }
    }
    
    return NULL;
}

struct node *parser_parse_statement(struct token **token_list)
{
    /* declaration_with_value | declaration_without_value | assignment | function_call */
    struct node *node;
    
    skip_newlines(token_list);

    static const struct {
        struct node *(*fn)(struct token **token_list);
    } statements[] = {
        { .fn = parser_parse_declaration_with_value },
        { .fn = parser_parse_declaration_without_value },
        { .fn = parser_parse_assignment },
        { .fn = parser_parse_function_call },
    };

    for (int i=0; i<LENGTH(statements); i++)
    {
        node = statements[i].fn(token_list);
        if (NULL != node)
        {
            return node;
        }
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
        if (skip_newlines(token_list))
        {
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
    /* function_arguments : VARTYPE STRING [COMMA] * */
    struct node *node, *next;
    struct token *tmp_list, *var;

    node = NULL;
    tmp_list = *token_list;
    if (0 == token_list_compare_all(&tmp_list, 2, VARTYPE, STRING))
    {
        token_list_step(&tmp_list);
        var = token_list_pop(&tmp_list);
        node = ast_function_argument_node_create(var);
        if (NULL == node)
        {
            DEBUG;
            return NULL;
        }
    }

    while (1)
    {
        if (0 == token_compare(tmp_list, COMMA))
        {
            token_list_step(&tmp_list);
            if (0 == token_list_compare_all(&tmp_list, 2, VARTYPE, STRING))
            {
                token_list_step(&tmp_list);
                var = token_list_pop(&tmp_list);
                next = ast_function_argument_node_create(var);
                if (0 == ast_node_append(&node, next))
                {
                    continue;
                }
            }
        }
        break;
    }

    *token_list = tmp_list;

    return node;
}

struct node *parser_parse_function_declaration(struct token **token_list)
{
    /* function_declaration : FUNC STRING LPAREN [function_arguments] RPAREN [VARTYPE]  
        [NEWLINE | SEMICOLON] LBRACE */
    struct token *name;
    struct node *node, *arguments;

    node = NULL;
    if (0 != token_list_compare_all(token_list, 3, FUNC, STRING, LPAREN))
    {   
        return NULL;
    }

    token_list_step(token_list);
    name = token_list_pop(token_list);
    
    node = ast_function_node_create(name);
    if (NULL == node)
    {
        DEBUG;
        return NULL;
    }
    token_list_step(token_list);

    while (0 != token_compare(*token_list, RPAREN))
    {
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
    token_list_step(token_list);
    
    skip_newlines(token_list);

    if (0 == token_compare(*token_list, VARTYPE))
    {
        token_list_step(token_list);
    }

    skip_newlines(token_list);

    if (0 != token_compare(*token_list, LBRACE))
    {
        DEBUG;
        return ast_destroy(node);
    }
    token_list_step(token_list);

    return node;
}

struct node *parser_parse_function(struct token **token_list)
{
    /*  function : function declaration [NEWLINE | SEMICOLON] assignment_list
     [NEWLINE | SEMICOLON] [RETURN] [NEWLINE | SEMICOLON] RBRACE */
    struct node *node, *body, *output;

    node = parser_parse_function_declaration(token_list);
    if (NULL == node)
    {
        return NULL;
    }

    if (skip_newlines(token_list))
    {
        body = parser_parse_statement_list(token_list);
        if (NULL != body)
        {
            ast_function_node_add_body(node, body);
        }
    }

    if (0 == token_compare(*token_list, RETURN))
    {
        token_list_step(token_list);
        output = parser_parse_expression(token_list);
        if (NULL == output)
        {
            DEBUG;
            return ast_destroy(node);
        }
        if ( 0 != ast_function_node_add_return(node, output))
        {
            DEBUG;
            return ast_destroy(output);
            return ast_destroy(node);
        }
    }

    skip_newlines(token_list);

    if (0 != token_compare(*token_list, RBRACE))
    {
        DEBUG;
        return ast_destroy(node);
    }
    token_list_step(token_list);

    return node;
}

struct node *parser_parse_function_list(struct token **token_list)
{
    /* function_list : function | (function [NEWLINE | SEMICOLON] function_list) */
    struct node *node, *next;

    node = parser_parse_function(token_list);
    if (NULL == node)
    {
        return NULL;
    }

    while (1)
    {
        if (skip_newlines(token_list))
        {
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
{ 
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