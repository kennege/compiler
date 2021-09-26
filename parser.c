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
    /* function_call : VARNAME OPEN_PAREN [factor | factor COMMA factor_list] CLOSE_PAREN */
    debug_print("%s, %s\n", __func__, token_get_display(*token_list));
    struct node *function_call, *param, *params;
    struct token *name, *tmp_list;

    function_call = NULL;
    tmp_list = *token_list;
    if (0 == token_list_compare_all(&tmp_list, 2, VARNAME, OPEN_PAREN))
    {
        name = token_list_pop(&tmp_list);
        token_list_step(&tmp_list);
        function_call = ast_function_call_node_create(name);
        params = parser_parse_factor(&tmp_list);

        while (0 == token_compare(tmp_list, COMMA))
        {
            token_list_step(&tmp_list);
            param = parser_parse_factor(&tmp_list);
            if (NULL == params)
            {
                break;
            }
            if (0 == ast_node_append(&params, param, NULL))
            {
                continue;
            }
            else
            {
                ERROR_MESSAGE;
                params = ast_node_destroy(params);
                function_call = ast_node_destroy(function_call);
                return ast_node_destroy(param);
            }
        }
    
        if (0 != token_compare(tmp_list, CLOSE_PAREN))
        {
            ERROR_MESSAGE;
            params = ast_node_destroy(params);
            function_call = ast_node_destroy(function_call);
            return ast_node_destroy(param);            
        }

        token_list_step(&tmp_list);
        ast_function_call_node_add_variables(function_call, params);
    }
    *token_list = tmp_list;

    return function_call;
}

struct node *parser_parse_factor(struct token **token_list)
{
    /* factor : VARNAME | ([NEWLINE | SEMICOLON] (PLUS | MINUS) (NUMBER | function_call | (OPEN_PAREN expression CLOSE_PAREN))) */
    debug_print("%s, %s\n", __func__, token_get_display(*token_list));
    struct node *expression, *function_call;
    struct token *token;

    expression = NULL;
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

    if (0 == token_compare(*token_list, VARNAME))
    {
        token = token_list_pop(token_list);
        return ast_variable_node_create(token);
    }
 
    if (0 == token_compare(*token_list, OPEN_PAREN))
    {
        token_list_step(token_list);
        expression = parser_parse_expression(token_list);
        if (NULL == expression)
        {
            ERROR_MESSAGE;
            return ast_node_destroy(expression);
        }

        if (0 != token_compare(*token_list, CLOSE_PAREN))
        {
            ERROR_MESSAGE;
            return ast_node_destroy(expression);
        }
        token_list_step(token_list);

        return expression;
    }

    return expression; 
}

struct node *parser_parse_term(struct token **token_list)
{   
    /* term : factor ((MULTIPLY | DIVIDE) factor) * */
    debug_print("%s, %s\n", __func__, token_get_display(*token_list));
    struct node *term;
    struct token *op;

    term = parser_parse_factor(token_list);
    if (NULL == term)
    {
        return NULL;
    }

    while (1)
    {
        if (0 == token_list_compare_any(token_list, 2, MULTIPLY, DIVIDE))
        {
            op = token_list_pop(token_list);
            term = ast_binary_node_create(term, op, parser_parse_factor(token_list));
            if (NULL == term)
            {
                ERROR_MESSAGE;
                return NULL;
            }
            continue;
        }
        break;
    }

    return term;
}

struct node *parser_parse_expression(struct token **token_list)
{
    /* expression : term (PLUS | MINUS) term * */
    debug_print("%s, %s\n", __func__, token_get_display(*token_list));
    struct node *expression;
    struct token *op;

    expression = parser_parse_term(token_list);
    if (NULL == expression)
    {
        return NULL;
    }
    
    while (1)
    {
        if (0 == token_list_compare_any(token_list, 2, MINUS, PLUS))
        {
            op = token_list_pop(token_list);
            expression = ast_binary_node_create(expression, op, parser_parse_term(token_list));
            if (NULL == expression)
            {
                ERROR_MESSAGE;
                return NULL;
            }
            continue;
        }
        break; 
    }

    return expression;
}

struct node *parser_parse_declaration_without_value(struct token **token_list)
{
    /* declaration_without_value : VAR VARNAME VARTYPE */
    debug_print("%s, %s\n", __func__, token_get_display(*token_list));
    struct node *variable;
    struct token *name, *tmp_list; 

    variable = NULL;
    tmp_list = *token_list;
    if (0 == token_compare(tmp_list, VAR))
    {
        token_list_step(&tmp_list);
        if (0 != token_compare(tmp_list, VARNAME))
        {
            ERROR_MESSAGE;
            return NULL;
        }
        name = token_list_pop(&tmp_list);
        variable = ast_variable_node_create(name);
        if (NULL == variable)
        {
            ERROR_MESSAGE;
            return NULL;
        }

        if (0 != token_compare(tmp_list, VARTYPE))
        {
            ERROR_MESSAGE;
            return ast_node_destroy(variable);
        }
        token_list_step(&tmp_list);
    } 
    *token_list = tmp_list;

    return ast_declaration_node_create(variable, token_create(ASSIGN, ASSIGN, 1), NULL);
}

struct node *parser_parse_declaration_with_value(struct token **token_list)
{
    /* declaration_with_value : VAR VARNAME VARTYPE EQUALS expression  */
    debug_print("%s, %s\n", __func__, token_get_display(*token_list));
    struct node *variable;
    struct token *name, *op, *tmp_list;   

    variable = NULL;
    tmp_list = *token_list;
    if (0 == token_compare(tmp_list, VAR))
    {
        token_list_step(&tmp_list);
        if (0 != token_compare(tmp_list, VARNAME))
        {
            ERROR_MESSAGE;
            return NULL;
        }
        
        name = token_list_pop(&tmp_list);      

        if (0 != token_compare(tmp_list, VARTYPE))
        {
            ERROR_MESSAGE;
            return NULL;
        }
        token_list_step(&tmp_list);
        
        if (0 != token_compare(tmp_list, EQUALS))
        {
            return NULL;
        }

        variable = ast_variable_node_create(name);
        if (NULL == variable)
        {
            ERROR_MESSAGE;
            return NULL;
        }

        op = token_list_pop(&tmp_list);
        *token_list = tmp_list;
        return ast_declaration_node_create(variable, op, parser_parse_expression(token_list));
    }

    return NULL;
}

struct node *parser_parse_assignment(struct token **token_list)
{
    /* assignment : (VARNAME EQUALS expression) | (VARNAME ASSIGN expression) | (VARNAME INCREMENT) | (VARNAME DECREMENT) */ 
    debug_print("%s, %s\n", __func__, token_get_display(*token_list));
    struct node *variable;
    struct token *name, *op, *tmp_list;

    variable = NULL;
    tmp_list = *token_list;
    if (0 == token_compare(tmp_list, VARNAME))
    {
        name = token_list_pop(&tmp_list);
        variable = ast_variable_node_create(name);
        if (0 == token_compare(*&tmp_list, EQUALS))
        {
            op = token_list_pop(&tmp_list);
            *token_list = tmp_list;
            return ast_assignment_node_create(variable, op, parser_parse_expression(token_list));     
        }
        if (0 == token_compare(tmp_list, ASSIGN))
        {
            op = token_list_pop(&tmp_list);
            *token_list = tmp_list;
            return ast_declaration_node_create(variable, op, parser_parse_expression(token_list));
        }
        if (0 == token_compare(tmp_list, INCREMENT))
        {
            op = token_list_pop(&tmp_list);
            *token_list = tmp_list;
            return ast_assignment_node_create(variable, token_create(EQUALS, EQUALS, 1), 
                ast_binary_node_create(variable, token_create(PLUS, PLUS, 1), ast_value_node_set(token_create(INT, "1", 1))));
        }
        if (0 == token_compare(tmp_list, DECREMENT))
        {
            op = token_list_pop(&tmp_list);
            *token_list = tmp_list;
            return ast_assignment_node_create(variable, token_create(EQUALS, EQUALS, 1), 
                ast_binary_node_create(variable, token_create(MINUS, MINUS, 1), ast_value_node_set(token_create(INT, "1", 1))));
        }        
    }
    
    return NULL;
}

struct node *parser_parse_comparison(struct token **token_list)
{
    /* comparison : [NOT] expression (LESS_THAN | LESS_EQUAL | EQUIVALENT | | NOT_EQUAL | GREATER_THAN | GREATER_EQUAL) expression */
    debug_print("%s, %s\n", __func__, token_get_display(*token_list));
    struct node *comparison, *left, *right;
    struct token *tmp_list, *not, *op;

    tmp_list = *token_list;
    not = NULL;
    if (0 == token_compare(tmp_list, NOT))
    {
        not = token_list_pop(&tmp_list);
    }

    left = parser_parse_expression(&tmp_list);
    if (NULL == left)
    {
        ERROR_MESSAGE;
        return NULL;
    }

    if (0 == token_list_compare_any(&tmp_list, 6, LESS_THAN, LESS_EQUAL, EQUIVALENT, NOT_EQUAL, GREATER_THAN, GREATER_EQUAL))
    {
        op = token_list_pop(&tmp_list);
    }
    else
    {
        ERROR_MESSAGE;
        return NULL;
    }
    
    right = parser_parse_expression(&tmp_list);
    if (NULL == right)
    {
        ERROR_MESSAGE;
        return NULL;
    }

    comparison = ast_comparison_node_create(not, left, op, right);
    if (NULL == comparison)
    {
        ERROR_MESSAGE;
        return NULL;
    }

    *token_list = tmp_list;

    return comparison;
}

struct node *parser_parse_comparison_list(struct token **token_list)
{
    /* comparison_list : comparison | (comparison (AND | OR) comparison_list) */ 
    debug_print("%s, %s\n", __func__, token_get_display(*token_list));
    struct node *comparison, *next_comparison;
    struct token *op, *tmp_list;

    tmp_list = *token_list;
    comparison = parser_parse_comparison(&tmp_list);
    if (NULL == comparison)
    {
        ERROR_MESSAGE;
        return NULL;
    }

    while (1)
    {
        if (0 == token_list_compare_any(&tmp_list, 2, AND, OR))
        {
            op = token_list_pop(&tmp_list);
            next_comparison = parser_parse_comparison(&tmp_list);
            if (NULL == next_comparison)
            {
                *token_list = tmp_list;
                return comparison;
            }
            if (0 == ast_node_append(&comparison, next_comparison, op))
            {
                continue;
            }  
            else
            {
                ERROR_MESSAGE;
            }       
        }
        break;
    }

    *token_list = tmp_list;

    return comparison;
}

struct node *parser_parse_condition_declaration(struct token **token_list)
{
    /* condition_declaration : (IF [OPEN_PAREN] comparison_list [CLOSE_PAREN]) | (ELSE [condition_delcaration]) */
    debug_print("%s, %s\n", __func__, token_get_display(*token_list));
    struct node *comparison, *condition;
    struct token *tmp_list, *op;

    condition = NULL;
    tmp_list = *token_list;
    if (0 == token_compare(tmp_list, IF))
    {
        op = token_list_pop(&tmp_list);
        if (0 == token_compare(tmp_list, OPEN_PAREN))
        {
            token_list_step(&tmp_list);
        }
        
        comparison = parser_parse_comparison_list(&tmp_list);
        if (NULL == comparison)
        {
            ERROR_MESSAGE;
            return NULL;
        }

        if (0 == token_compare(tmp_list, CLOSE_PAREN))
        {
            token_list_step(&tmp_list);
        }

        condition = ast_condition_node_create(comparison, op);
        if (NULL == condition)
        {
            ERROR_MESSAGE;
            return NULL;
        }
    }
    else if (0 == token_compare(tmp_list, ELSE))
    {
        op = token_list_pop(&tmp_list);
        condition = parser_parse_condition_declaration(&tmp_list);
        if (NULL == condition)
        {
            *token_list = tmp_list;
            return ast_condition_node_create(NULL, op);
        }
    }

    *token_list = tmp_list;

    return condition;
}

struct node *parser_parse_condition(struct token **token_list)
{
    /* condition : condition_declaration [newline] OPEN_BRACE [newline] statement_list [newline] CLOSE_BRACE  */
    debug_print("%s, %s\n", __func__, token_get_display(*token_list));
    struct node *condition, *statement_list;
    struct token *tmp_list;

    tmp_list = *token_list;
    condition = parser_parse_condition_declaration(&tmp_list);     
    if (NULL == condition)
    {
        return NULL;
    }
    skip_newlines(&tmp_list);

    if (0 != token_compare(tmp_list, OPEN_BRACE))
    {
        ERROR_MESSAGE;
        return NULL;
    }
    token_list_step(&tmp_list);
    skip_newlines(&tmp_list);

    statement_list = parser_parse_statement_list(&tmp_list);
    if (NULL == statement_list)
    {
        ERROR_MESSAGE;
        return NULL;
    }
    skip_newlines(&tmp_list);

    if (0 != token_compare(tmp_list, CLOSE_BRACE))
    {
        ERROR_MESSAGE;
        return NULL;
    }
    token_list_step(&tmp_list);

    if (0 != ast_condition_node_add_body(condition, statement_list))
    {
        ERROR_MESSAGE;
        return NULL;
    }

    *token_list = tmp_list;

    return condition;
}

struct node *parser_parse_condition_list(struct token **token_list)
{
    /* condition_list : condition | (statement NEWLINE condition_list) */ 
    debug_print("%s, %s\n", __func__, token_get_display(*token_list));
    struct node *condition, *next_condition;

    condition = parser_parse_condition(token_list);
    if (NULL == condition)
    {
        return NULL;
    }

    while (1)
    {
        next_condition = parser_parse_condition(token_list);            
        if (NULL == next_condition)
        {
            break;
        }
        if (0 == ast_node_append(&condition->body, next_condition, NULL))
        {
            continue;
        }  
        else
        {
            ERROR_MESSAGE;
        }       
        break;
    }

    return condition;
}

struct node *parser_parse_loop_declaration(struct token **token_list)
{
    /* loop_declaration : FOR [OPEN_PAREN] declaration_with_value SEMICOLON comparison SEMICOLON assignment [CLOSE_PAREN] */
    debug_print("%s, %s\n", __func__, token_get_display(*token_list));
    struct node *declaration, *comparison, *assignment, *loop_declaration;
    struct token *tmp_list, *op;

    tmp_list = *token_list;
    if (0 != token_compare(tmp_list, FOR))
    {
        return NULL;
    }
    op = token_list_pop(&tmp_list);

    if (0 == token_compare(tmp_list, OPEN_PAREN))
    {
        token_list_step(&tmp_list);
    }

    declaration = parser_parse_assignment(&tmp_list);
    if (NULL == declaration)
    {
        ERROR_MESSAGE;
        return NULL;
    }

    if (0 != token_compare(tmp_list, SEMICOLON))
    {
        ERROR_MESSAGE;
        return NULL;
    }
    token_list_step(&tmp_list);

    comparison = parser_parse_comparison(&tmp_list);
    if (NULL == comparison)
    {
        ERROR_MESSAGE;
        return ast_node_destroy(declaration);
    }

    if (0 != token_compare(tmp_list, SEMICOLON))
    {
        ERROR_MESSAGE;
        return NULL;
    }
    token_list_step(&tmp_list);

    assignment = parser_parse_assignment(&tmp_list);
    if (NULL == assignment)
    {
        ERROR_MESSAGE;
        declaration = ast_node_destroy(declaration);
        return ast_node_destroy(comparison);
    }

    if (0 == token_compare(tmp_list, CLOSE_PAREN))
    {
        token_list_step(&tmp_list);
    }

    loop_declaration = ast_loop_node_create(declaration, comparison, assignment, op);
    if (NULL == loop_declaration)
    {
        ERROR_MESSAGE;
        return NULL;
    }

    *token_list = tmp_list;

    return loop_declaration;
}

struct node *parser_parse_loop(struct token **token_list)
{
    /* loop : loop_declaration [NEWLINE] OPEN_BRACE STATEMENT_LIST [NEWLINE] CLOSE_BRACE */
    debug_print("%s, %s\n", __func__, token_get_display(*token_list));
    struct node *loop_declaration, *statement_list;
    struct token *tmp_list;

    tmp_list = *token_list;
    loop_declaration = parser_parse_loop_declaration(&tmp_list);
    if (NULL == loop_declaration)
    {
        return NULL;
    }
    skip_newlines(&tmp_list);
    
    if (0 != token_compare(tmp_list, OPEN_BRACE))
    {
        ERROR_MESSAGE;
        return ast_node_destroy(loop_declaration);
    }
    token_list_step(&tmp_list);

    statement_list = parser_parse_statement_list(&tmp_list);
    if (NULL == statement_list)
    {
        ERROR_MESSAGE;
        return ast_node_destroy(loop_declaration);
    }
    skip_newlines(&tmp_list);

    if (0 != token_compare(tmp_list, CLOSE_BRACE))
    {
        ERROR_MESSAGE;
        statement_list = ast_node_destroy(statement_list);
        return ast_node_destroy(loop_declaration);
    }
    token_list_step(&tmp_list);

    if (0 != ast_loop_node_add_body(loop_declaration, statement_list))
    {
        ERROR_MESSAGE;
        statement_list = ast_node_destroy(statement_list);
        return ast_node_destroy(loop_declaration);       
    }

    *token_list = tmp_list;

    return loop_declaration;
}

struct node *parser_parse_statement(struct token **token_list)
{
    /* statement : declaration_with_value | declaration_without_value | assignment | function_call | condition | loop */
    debug_print("%s, %s\n", __func__, token_get_display(*token_list));
    struct node *statement;
    struct token *tmp_list;
    
    tmp_list = *token_list;
    skip_newlines(&tmp_list);

    static const struct {
        struct node *(*fn)(struct token **token_list);
    } statements[] = {
        { .fn = parser_parse_declaration_with_value },
        { .fn = parser_parse_declaration_without_value },
        { .fn = parser_parse_assignment },
        { .fn = parser_parse_function_call },
        { .fn = parser_parse_condition_list },
        { .fn = parser_parse_loop },
    };

    for (int i=0; i<LENGTH(statements); i++)
    {
        statement = statements[i].fn(&tmp_list);
        if (NULL != statement)
        {
            debug_print("statement: %s, %ld\n", statement->type, ast_num_nodes(statement));
            *token_list = tmp_list;
            return statement;
        }
    }

    return NULL;
}   

struct node *parser_parse_statement_list(struct token **token_list)
{
    /* statement_list : statement | (statement (NEWLINE | SEMICOLON) statement_list) */ 
    debug_print("%s, %s\n", __func__, token_get_display(*token_list));
    struct node *statement, *next_statement;

    statement = parser_parse_statement(token_list);
    if (NULL == statement)
    {
        return NULL;
    }

    while (1)
    {
        if (skip_newlines(token_list))
        {
            next_statement = parser_parse_statement(token_list);            
            if (NULL == next_statement)
            {
                return statement;
            }
            if (0 == ast_node_append(&statement, next_statement, NULL))
            {
                continue;
            }  
            else
            {
                ERROR_MESSAGE;
            }       
        }
        break;
    }

    return statement;
}

struct node *parser_parse_function_arguments(struct token **token_list)
{
    /* function_arguments : VARTYPE VARNAME [COMMA] * */
    debug_print("%s, %s\n", __func__, token_get_display(*token_list));
    struct node *argument, *next_argument;
    struct token *tmp_list, *var;

    argument = NULL;
    tmp_list = *token_list;
    if (0 == token_list_compare_all(&tmp_list, 2, VARTYPE, VARNAME))
    {
        token_list_step(&tmp_list);
        var = token_cpy(token_list_pop(&tmp_list));
        argument = ast_function_argument_node_create(var);
        if (NULL == argument)
        {
            ERROR_MESSAGE;
            return NULL;
        }
    }

    while (1)
    {
        if (0 == token_compare(tmp_list, COMMA))
        {
            token_list_step(&tmp_list);
            if (0 == token_list_compare_all(&tmp_list, 2, VARTYPE, VARNAME))
            {
                token_list_step(&tmp_list);
                var = token_cpy(token_list_pop(&tmp_list));
                next_argument = ast_function_argument_node_create(var);
                if (0 == ast_node_append(&argument, next_argument, NULL))
                {
                    continue;
                }
            }
        }
        break;
    }

    *token_list = tmp_list;

    return argument;
}

struct node *parser_parse_function_declaration(struct token **token_list)
{
    /* function_declaration : FUNC VARNAME OPEN_PAREN [function_arguments] CLOSE_PAREN [VARTYPE]  
        [NEWLINE | SEMICOLON] OPEN_BRACE */
    debug_print("%s, %s\n", __func__, token_get_display(*token_list));
    struct token *function_name;
    struct node *function_declaration, *arguments;

    function_declaration = NULL;
    if (0 != token_list_compare_all(token_list, 3, FUNC, VARNAME, OPEN_PAREN))
    {   
        return NULL;
    }

    token_list_step(token_list);
    function_name = token_list_pop(token_list);
    
    function_declaration = ast_function_node_create(function_name);
    if (NULL == function_declaration)
    {
        ERROR_MESSAGE;
        return NULL;
    }
    token_list_step(token_list);

    while (0 != token_compare(*token_list, CLOSE_PAREN))
    {
        arguments = parser_parse_function_arguments(token_list);
        if (NULL != arguments)
        {
            if (0 != ast_function_node_add_arguments(function_declaration, arguments))
            {
                ERROR_MESSAGE;
                arguments = ast_node_destroy(arguments);
                return ast_node_destroy(function_declaration);
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

    if (0 != token_compare(*token_list, OPEN_BRACE))
    {
        ERROR_MESSAGE;
        return ast_node_destroy(function_declaration);
    }
    token_list_step(token_list);

    return function_declaration;
}

struct node *parser_parse_function(struct token **token_list)
{
    /*  function : function declaration [NEWLINE | SEMICOLON] assignment_list
     [NEWLINE | SEMICOLON] [RETURN] [NEWLINE | SEMICOLON] CLOSE_BRACE */
    debug_print("%s, %s\n", __func__, token_get_display(*token_list));
    struct node *function, *function_body, *function_return;
    struct token *tmp_list;

    tmp_list = *token_list;
    function = parser_parse_function_declaration(&tmp_list);
    if (NULL == function)
    {
        return NULL;
    }

    if (skip_newlines(&tmp_list))
    {
        function_body = parser_parse_statement_list(&tmp_list);
        if (NULL != function_body)
        {
            ast_function_node_add_body(function, function_body);
        }
    }

    if (0 == token_compare(tmp_list, RETURN))
    {
        token_list_step(&tmp_list);
        function_return = parser_parse_expression(&tmp_list);
        if (NULL == function_return)
        {
            ERROR_MESSAGE;
            return ast_node_destroy(function);
        }
        if ( 0 != ast_function_node_add_return(function, function_return))
        {
            ERROR_MESSAGE;
            return ast_node_destroy(function_return);
            return ast_node_destroy(function);
        }
    }

    skip_newlines(&tmp_list);
    if (0 != token_compare(tmp_list, CLOSE_BRACE))
    {
        ERROR_MESSAGE;
        return ast_node_destroy(function);
    }
    token_list_step(&tmp_list);

    *token_list = tmp_list;

    return function;
}

struct node *parser_parse_function_list(struct token **token_list)
{
    /* function_list : function | (function [NEWLINE | SEMICOLON] function_list) */
    struct node *function, *next_function;

    function = parser_parse_function(token_list);
    if (NULL == function)
    {
        return NULL;
    }

    while (1)
    {
        if (skip_newlines(token_list))
        {
            next_function = parser_parse_function(token_list);
            if (NULL == next_function)
            {
                return function;
            }
            if (0 == ast_node_append(&function, next_function, NULL))
            {
                continue;
            }
        }
        break;
    }

    return function;
}

struct node *parser_parse_program(struct token **token_list)
{ 
    /* program : [NEWLINE] function_list | assignment_list | expression */

    struct node *program;

    skip_newlines(token_list);

    program = parser_parse_function_list(token_list);
    if (NULL != program)
    {
        return program;
    }

    program = parser_parse_statement_list(token_list);
    if (NULL != program)
    {
        return program;
    }

    return parser_parse_expression(token_list);
}

struct node *parser_parse(struct token *token_list)
{    
    struct node *program;

    if (NULL == token_list || 0 == token_compare(token_list, EOFILE_TYPE))
    {
        ERROR_MESSAGE;
        return NULL;
    }

    program = parser_parse_program(&token_list);
    if (NULL == program)
    {
        ERROR_MESSAGE;
        return NULL;
    }

    if (0 < token_list_length(token_list))
    {
        fprintf(stderr, "parsing not completed ");
        ERROR_MESSAGE;
        return NULL;
    }
 
    return program;
}