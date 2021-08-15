#ifndef _LEXER_H_
#define _LEXER_H_

struct token *lexer_eat(const char *type, struct token **token_list);
struct token *lexer_lex(char *input_str);

/* Arithmetic */
#define INTEGER "INTEGER"
#define PLUS "PLUS"
#define MINUS "MINUS"
#define MULTIPLY "MULTIPLY"
#define DIVIDE "DIVIDE"
#define LPAREN "LPAREN"
#define RPAREN "RPAREN"

/* Misc */
#define EOFILE "EOFILE"
#define WHITESPACE "WHITESPACE"

#endif // _LEXER_H_