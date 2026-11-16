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
#define MAXARGS 50
#define MAXCMDS 10
#define MAXJOBS 32
#define HISTORY_SIZE 20
#define PROMPT "FCIT> "

typedef struct {
    char* argv[MAXARGS + 1];
    char* infile;
    char* outfile;
} Command;

char** tokenize(char* cmdline);
int parse_commands(char** arglist, Command* cmds);

pid_t execute_pipeline(Command* cmds, int n, int background,
                       int* exit_status);

int handle_builtin(char** arglist);

void hist_add(const char* cmd);
char* hist_get(int n);
void hist_print(void);

char* join_tokens(char** seg);

void job_add(pid_t pid, char* label);
void reap_jobs(void);
void jobs_print(void);

#endif
