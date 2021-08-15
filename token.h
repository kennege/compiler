#ifndef _TOKEN_H_
#define _TOKEN_H_

struct token;

struct window
{
    int left;
    int right;
};

int token_save(const char *type, const char *input_str, int value_len, struct token **head);

#endif // _TOKEN_H_