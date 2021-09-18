#ifndef _PARSER_H_
#define _PARSER_H_

struct node *parser_parse(struct token *token_list);

/* function_call : VARNAME OPEN_PAREN [factor | factor COMMA factor_list] CLOSE_PAREN */

/* factor : VARNAME | ([NEWLINE | SEMICOLON] (PLUS | MINUS) (NUMBER | function_call | (OPEN_PAREN expression CLOSE_PAREN))) */

/* term : factor ((MULTIPLY | DIVIDE) factor) * */

/* expression : term (PLUS | MINUS) term * */

/* declaration_without_value : VAR VARNAME VARTYPE */

/* declaration_with_value : VAR VARNAME VARTYPE EQUALS expression  */

/* assignment : (VARNAME EQUALS expression) | (VARNAME ASSIGN expression) | (VARNAME INCREMENT) | (VARNAME DECREMENT) */ 

/* comparison : [NOT] expression (LESS_THAN | LESS_EQUAL | EQUIVALENT | | NOT_EQUAL | GREATER_THAN | GREATER_EQUAL) expression */

/* comparison_list : comparison | (comparison (AND | OR) comparison_list) */ 

/* condition_declaration : (IF [OPEN_PAREN] comparison_list [CLOSE_PAREN]) | (ELSE [condition_delcaration]) */

/* condition : condition_declaration [newline] OPEN_BRACE [newline] statement_list [newline] CLOSE_BRACE  */

/* condition_list : condition | (statement NEWLINE condition_list) */ 

/* loop_declaration : FOR [OPEN_PAREN] declaration_with_value SEMICOLON comparison SEMICOLON assignment [CLOSE_PAREN] */

/* loop : loop_declaration [NEWLINE] OPEN_BRACE STATEMENT_LIST [NEWLINE] CLOSE_BRACE */

/* statement : declaration_with_value | declaration_without_value | assignment | function_call | condition | loop */

/* statement_list : statement | (statement (NEWLINE | SEMICOLON) statement_list) */ 

/* function_arguments : VARTYPE VARNAME [COMMA] * */

/* function_declaration : FUNC VARNAME OPEN_PAREN [function_arguments] CLOSE_PAREN [VARTYPE]  
    [NEWLINE | SEMICOLON] OPEN_BRACE */

/*  function : function declaration [NEWLINE | SEMICOLON] assignment_list
    [NEWLINE | SEMICOLON] [RETURN] [NEWLINE | SEMICOLON] CLOSE_BRACE */

/* function_list : function | (function [NEWLINE | SEMICOLON] function_list) */

/* program : function_list | assignment_list | expression */
    
#endif // _PARSER_H_