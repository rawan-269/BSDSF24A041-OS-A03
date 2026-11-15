#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define MAX_LEN 512
#define MAXARGS 20
#define MAXCMDS 10
#define HISTORY_SIZE 20
#define PROMPT "FCIT> "

typedef struct {
    char* argv[MAXARGS + 1];
    char* infile;
    char* outfile;
} Command;

char** tokenize(char* cmdline);
int parse_commands(char** arglist, Command* cmds);
int execute_pipeline(Command* cmds, int n);
int handle_builtin(char** arglist);
void hist_add(const char* cmd);
char* hist_get(int n);
void hist_print(void);

#endif
