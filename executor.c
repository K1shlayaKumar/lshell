#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>  // piece of shit
#include <unistd.h>
#include <sys/wait.h>

#include "executor.h"
#include "tokenizer.h"  

/* from fcntl.h
O_RDONLY	            open for reading only
O_WRONLY	            open for writing only
O_CREAT	                create the file if it doesn’t exist (needs the permission argument, 0644)
O_TRUNC	                if the file exists, empty it first
O_APPEND	            every write goes to the end of the file
*/

// don't ask how long it took me to find out about 'static' 
static int syntax_ok(char **tokens) {
    int words = 0;

    for (int i = 0; tokens[i] != NULL; i++) {
        if (strcmp(tokens[i], "|") == 0) {
            if (words == 0) {
                fprintf(stderr, "lshell: syntax error\n"); // any input with nothing or space before a pipe is bullshit
                return 0; 
            }
            words = 0;
        } else if (is_operator(tokens[i])) {
            if (tokens[i + 1] == NULL || is_operator(tokens[i + 1])) {
                fprintf(stderr, "lshell: syntax error\n"); // other operators followed by NULL or another operator is also bullshit
                return 0;
            }
            i++;
        } else {
            words++;
        }
    }
    return 1;
}

static void apply_redirections(char **argv) {
    int n = 0;

    for (int j = 0; argv[j] != NULL; j++) {
        int file; 
        int target; // which standard stream to replace

        if (strcmp(argv[j], "<") == 0) {
            file = open(argv[j + 1], O_RDONLY);
            target = STDIN_FILENO;
        } else if (strcmp(argv[j], ">") == 0) {
            file = open(argv[j + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            target = STDOUT_FILENO;
        } else if (strcmp(argv[j], ">>") == 0) {
            file = open(argv[j + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
            target = STDOUT_FILENO;
        } else {
            argv[n] = argv[j];
            n++;
            continue;
        }

        if (file < 0) {
            perror(argv[j + 1]);
            _exit(1);
        }
        dup2(file, target);
        close(file);
        j++;
    }

    argv[n] = NULL;
}

void run_pipeline(char **tokens) {
    if (!syntax_ok(tokens)) return;

    int start = 0;
    int end = -1;
    int children = 0;

    // don't ask me why the condition always needs to be true
    for (int i = 0; 1; i++) {
        int at_end = (tokens[i] == NULL);
        int at_pipe = !at_end && strcmp(tokens[i], "|") == 0;
        if (!at_end && !at_pipe) continue;

        int file[2] = {-1, -1};
        if (at_pipe && pipe(file) < 0) {
            perror("pipe");
            break;
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            if (at_pipe) {
                close(file[0]);
                close(file[1]);
            }
            break;
        }

        if (pid == 0) {
            signal(SIGINT, SIG_DFL); // job control or smth

            if (end != -1) {
                dup2(end, STDIN_FILENO);
                close(end);
            }
            if (at_pipe) {
                dup2(file[1], STDOUT_FILENO);
                close(file[0]);
                close(file[1]);
            }

            tokens[i] = NULL;
            char **argv = &tokens[start];

            apply_redirections(argv);
            execvp(argv[0], argv);

            perror(argv[0]);
            _exit(1);
        }

        children++;
        if (end != -1) close(end);
        if (at_pipe) {
            close(file[1]);
            end = file[0];
        } else {
            end = -1;
        }

        start = i + 1;
        if (at_end) break;
    }

    if (end != -1) close(end);

    for (int k = 0; k < children; k++) {
        wait(NULL);
    }
}
