#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <readline/history.h>
#include <readline/readline.h>

#include "tokenizer.h"
#include "prompt.h"
#include "builtins.h"
#include "executor.h"

int main(void) {
    char *line;
    char prompt[4200];

    signal(SIGINT, SIG_IGN);

    while (1) {
        make_prompt(prompt, sizeof(prompt));
        line = readline(prompt);
        if (line == NULL) break;

        if (line[0] != '\0') {
            add_history(line);
        }

        char **tokens = tokenize(line);
        int quit = 0;

        if (tokens != NULL && tokens[0] != NULL) {
            if (strcmp(tokens[0], "exit") == 0) {
                quit = 1;
            } else if (strcmp(tokens[0], "cd") == 0) {
                change_directory(tokens);
            } else {
                run_pipeline(tokens);
            }
        }

        if (tokens != NULL) {
            free_tokens(tokens);
        }
        free(line);

        if (quit) break;
    }

    printf("exit\n");
    return 0;
}
