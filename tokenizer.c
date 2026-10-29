#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "tokenizer.h"

int is_operator_char(char c) {
    return c == '|' || c == '<' || c == '>';
    }
    

int is_operator(const char *s) {
    return strcmp(s, "|") == 0 || strcmp(s, "<") == 0 || strcmp(s, ">") == 0 || strcmp(s, ">>") == 0;
}

char **tokenize(const char *line) {
    size_t capacity = 8;
    size_t count = 0;
    char **tokens = malloc(capacity * sizeof(char *));
    char *buf = malloc(strlen(line) + 1); // We need extra buffer for '\0' terminator


    //In case the functions meant to allocate memory fails to allocate memory, I will never not find this funny lol
    if (tokens == NULL || buf == NULL) {
        free(tokens);
        free(buf);
        return NULL;
    }

    tokens[0] = NULL;

    const char *p = line;
    while (*p != '\0') {
        while (isspace((unsigned char)*p)) p++; //unsigned char is something isspace needs
        if (*p == '\0') break;

        size_t len = 0;

        if (is_operator_char(*p)) {
            buf[len] = *p;
            len++;
            if (*p == '>' && p[1] == '>') { 
                buf[len] = '>';
                len++;
                p++;
            }
            p++;
        } else {
            while (*p != '\0' && !isspace((unsigned char)*p) && !is_operator_char(*p)) {
                if (*p == '\'' || *p == '"') {
                    char quote = *p;
                    p++;
                    while (*p != '\0' && *p != quote) {
                        buf[len] = *p;
                        len++;
                        p++;
                    }
                    if (*p == '\0') {
                        fprintf(stderr, "lshell: unclosed quote\n");
                        free_tokens(tokens);
                        free(buf);
                        return NULL;
                    }
                    p++;
                } else {
                    buf[len] = *p;
                    len++;
                    p++;
                }
            }
        }
        buf[len] = '\0';

        // typical dynamic array implementation
        if (count + 1 >= capacity) {
            capacity *= 2;
            char **bigger = realloc(tokens, capacity * sizeof(char *));
            if (bigger == NULL) {
                free_tokens(tokens);
                free(buf);
                return NULL;
            }
            tokens = bigger;
        }

        tokens[count] = strdup(buf);
        count++;
        tokens[count] = NULL;
    }

    free(buf);
    return tokens;
}

void free_tokens(char **tokens) {
    for (size_t i = 0; tokens[i] != NULL; i++)
        free(tokens[i]);
    free(tokens);
}
