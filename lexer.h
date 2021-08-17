#ifndef _LEXER_H_
#define _LEXER_H_

struct token *lexer_eat(const char *type, struct token **token_list);
struct token *lexer_lex(char *input_str);

/* Reserved Keywords */
#define ASSIGN ":="
#define CONST "const"
#define FUNC "func"
#define VAR "var"
#define INT "int"
#define BOOL "bool"
#define FLOAT "float32"
#define STRING "string"
#define EOFILE "/0"

/* Reserved Characters */
#define PLUS "+"
#define MINUS "-"
#define MULTIPLY "*"
#define DIVIDE "/"
#define LPAREN "("
#define RPAREN ")"
#define LBRACE "{"
#define RBRACE "}"
#define WHITESPACE " "

/* Forbidden Characters */


/* Numbers */
#define INTEGER "0123456789"

/* Characters */
#define CHARACTERS "abcdefghijklmnopqrstuvwxyz"

#endif // _LEXER_H_