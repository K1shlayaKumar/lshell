#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "prompt.h"

// making the ~/Downloads or ~/Documents type of prompt instead of the entire
// path all the time
void make_prompt(char *prompt, size_t size) {
  char pwd[4096];
  char *home = getenv("HOME");

  getcwd(pwd, sizeof(pwd));

  if (home != NULL && strcmp(pwd, home) == 0) {
    snprintf(prompt, size, "lshell> ");
  } else if (home != NULL && strncmp(pwd, home, strlen(home)) == 0 &&
             pwd[strlen(home)] == '/') {
    snprintf(prompt, size, "~%s> ", pwd + strlen(home));
  } else {
    snprintf(prompt, size, "%s> ", pwd);
  }
}
