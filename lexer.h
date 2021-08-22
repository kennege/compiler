#ifndef _LEXER_H_
#define _LEXER_H_

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

/* Reserved Characters */
#define PLUS "+"
#define MINUS "-"
#define MULTIPLY "*"
#define DIVIDE "/"
#define LPAREN "("
#define RPAREN ")"
#define LBRACE "{"
#define RBRACE "}"
#define EQUALS "="
#define WHITESPACE " "

/* Special Characters */
#define EOFILE "\0"
#define NEWLINE "\n"

/* Descriptors */
#define WORD "__word__"
#define VARTYPE "__vartype__"
#define NEWLINE_TYPE "__newline__"
#define EOFILE_TYPE "__eofile__"

/* Numbers */
#define INTEGER "0123456789"

/* Characters */
#define CHARACTER "abcdefghijklmnopqrstuvwxyz"

#endif // _LEXER_H_