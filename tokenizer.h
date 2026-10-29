#ifndef TOKENIZER_H
#define TOKENIZER_H


char **tokenize(const char *line);
void free_tokens(char **tokens);

int is_operator_char(char c);
int is_operator(const char *s);

#endif
