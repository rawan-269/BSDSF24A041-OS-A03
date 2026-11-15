#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define MAX_LEN 512
#define MAXARGS 10
#define ARGLEN 30
#define HISTORY_SIZE 20
#define PROMPT "FCIT> "

char** tokenize(char* cmdline);
int execute(char** arglist);
int handle_builtin(char** arglist);
void hist_add(const char* cmd);
char* hist_get(int n);
void hist_print(void);

#endif
