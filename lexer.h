#ifndef _LEXER_H_
#define _LEXER_H_

struct token *lexer_lex(char *input_str);

/* Reserved Keywords */
#define DOUBLE_QUOTE "'"
#define COMMENT "//"
#define ASSIGN ":="
#define CONST "const"
#define FUNC "func"
#define MAIN "main"
#define VAR "var"
#define AND "&&"
#define OR "||"
#define IF "if"
#define FOR "for"
#define ELSE "else"
#define RETURN "return"
#define INCREMENT "++"
#define DECREMENT "--"
#define NOT_EQUAL "!="
#define EQUIVALENT "=="
#define LESS_EQUAL "<="
#define GREATER_EQUAL ">="

/* Variable Types */
#define INT "int"
#define FLOAT "float32"
#define STRING "string"

/* Reserved Characters */
#define PLUS "+"
#define MINUS "-"
#define MULTIPLY "*"
#define DIVIDE "/"
#define OPEN_PAREN "("
#define CLOSE_PAREN ")"
#define OPEN_BRACE "{"
#define CLOSE_BRACE "}"
#define EQUALS "="
#define COMMA ","
#define DOT "."
#define NOT "!"
#define SEMICOLON ";"
#define WHITESPACE " "
#define LESS_THAN "<"
#define GREATER_THAN ">"

/* Special Characters */
#define EOFILE "\0"
#define NEWLINE "\n"

/* Descriptors */
#define VARTYPE "__vartype__"
#define VARNAME "__varname__"
#define NEWLINE_TYPE "__newline__"
#define EOFILE_TYPE "__eofile__"

/* Numbers */
#define NUMBERS "0123456789"

/* Characters */
#define CHARACTERS "abcdefghijklmnopqrstuvwxyz_"

#endif // _LEXER_H_