#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "builtins.h"

void change_directory(char **argv) {
    char *dir = argv[1];
    if (dir == NULL) {
        dir = getenv("HOME");
    }
    if (dir == NULL) {
        fprintf(stderr, "cd: HOME not set\n");
    } else if (chdir(dir) != 0) {
        perror("cd");
    }
}
