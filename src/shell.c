#include "shell.h"

typedef struct {
    pid_t pid;
    char* label;
} Job;

static char* history[HISTORY_SIZE];
static int history_count = 0;
static Job jobs[MAXJOBS];
static int job_count = 0;

static int is_op(char c) {
    return c == '<' || c == '>' || c == '|' || c == ';' || c == '&';
}

char** tokenize(char* cmdline) {
    if (cmdline == NULL || cmdline[0] == '\0' || cmdline[0] == '\n') {
        return NULL;
    }

    char** arglist = (char**)calloc(MAXARGS + 1, sizeof(char*));
    char* cp = cmdline;
    int argnum = 0;

    while (*cp != '\0' && argnum < MAXARGS) {
        while (*cp == ' ' || *cp == '\t') cp++;

        if (*cp == '\0') break;

        char* start = cp;
        if (is_op(*cp)) {
            cp++;
        } else {
            while (*cp != '\0' && *cp != ' ' && *cp != '\t' && !is_op(*cp)) {
                cp++;
            }
        }

        int len = cp - start;
        arglist[argnum] = (char*)malloc(len + 1);
        memcpy(arglist[argnum], start, len);
        arglist[argnum][len] = '\0';
        argnum++;
    }

    if (argnum == 0) {
        free(arglist);
        return NULL;
    }

    return arglist;
}

int parse_commands(char** arglist, Command* cmds) {
    memset(cmds, 0, sizeof(Command) * MAXCMDS);
    int c = 0;
    int argc = 0;

    for (int i = 0; arglist[i] != NULL; i++) {
        char* t = arglist[i];

        if (strcmp(t, "|") == 0) {
            if (argc == 0) {
                fprintf(stderr, "syntax error near '|'\n");
                return -1;
            }
            if (c + 1 >= MAXCMDS) {
                fprintf(stderr, "too many commands in pipeline\n");
                return -1;
            }
            c++;
            argc = 0;
        }
        else if (strcmp(t, "<") == 0 || strcmp(t, ">") == 0) {
            char* next = arglist[i + 1];
            if (next == NULL || strcmp(next, "|") == 0 ||
                strcmp(next, "<") == 0 || strcmp(next, ">") == 0) {
                fprintf(stderr, "syntax error: missing file after '%s'\n", t);
                return -1;
            }
            if (t[0] == '<') {
                cmds[c].infile = next;
            } else {
                cmds[c].outfile = next;
            }
            i++;
        }
        else {
            cmds[c].argv[argc++] = t;
        }
    }

    if (argc == 0) {
        fprintf(stderr, "syntax error: missing command\n");
        return -1;
    }

    return c + 1;
}

void hist_add(const char* cmd) {
    if (history_count == HISTORY_SIZE) {
        free(history[0]);
        memmove(history, history + 1, (HISTORY_SIZE - 1) * sizeof(char*));
        history_count--;
    }
    history[history_count++] = strdup(cmd);
}

char* hist_get(int n) {
    if (n < 1 || n > history_count) return NULL;
    return history[n - 1];
}

void hist_print(void) {
    for (int i = 0; i < history_count; i++) {
        printf("%d  %s\n", i + 1, history[i]);
    }
}

char* join_tokens(char** seg) {
    size_t total = 1;
    for (int i = 0; seg[i] != NULL; i++) {
        total += strlen(seg[i]) + 1;
    }
    char* s = (char*)malloc(total);
    s[0] = '\0';
    for (int i = 0; seg[i] != NULL; i++) {
        if (i > 0) strcat(s, " ");
        strcat(s, seg[i]);
    }
    return s;
}

void job_add(pid_t pid, char* label) {
    if (job_count == MAXJOBS) {
        fprintf(stderr, "too many background jobs\n");
        free(label);
        return;
    }
    jobs[job_count].pid = pid;
    jobs[job_count].label = label;
    job_count++;
    printf("[%d] started: %s\n", pid, label);
}

void reap_jobs(void) {
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        for (int i = 0; i < job_count; i++) {
            if (jobs[i].pid == pid) {
                printf("[%d] done: %s\n", pid, jobs[i].label);
                free(jobs[i].label);
                memmove(&jobs[i], &jobs[i + 1], (job_count - i - 1) * sizeof(Job));
                job_count--;
                break;
            }
        }
    }
}

void jobs_print(void) {
    if (job_count == 0) {
        printf("No background jobs.\n");
        return;
    }
    for (int i = 0; i < job_count; i++) {
        printf("[%d] %d  Running  %s\n", i + 1, jobs[i].pid, jobs[i].label);
    }
}

int handle_builtin(char** arglist) {
    if (arglist[0] == NULL) return 0;

    if (strcmp(arglist[0], "exit") == 0) {
        printf("Exiting shell...\n");
        exit(0);
    }
    else if (strcmp(arglist[0], "cd") == 0) {
        if (arglist[1] == NULL) {
            fprintf(stderr, "cd: expected argument\n");
        } else if (chdir(arglist[1]) != 0) {
            perror("cd failed");
        }
        return 1;
    }
    else if (strcmp(arglist[0], "help") == 0) {
        printf("Built-in commands:\n");
        printf("  cd <dir>  - change directory\n");
        printf("  exit      - exit the shell\n");
        printf("  help      - show this help\n");
        printf("  jobs      - list running background jobs\n");
        printf("  history   - show command history\n");
        printf("  !n        - re-run the n-th command from history\n");
        printf("Operators:\n");
        printf("  cmd > file   - redirect output to file\n");
        printf("  cmd < file   - take input from file\n");
        printf("  cmd1 | cmd2  - pipe output of cmd1 into cmd2\n");
        printf("  cmd1 ; cmd2  - run commands one after another\n");
        printf("  cmd &        - run command in the background\n");
        return 1;
    }
    else if (strcmp(arglist[0], "jobs") == 0) {
        reap_jobs();
        jobs_print();
        return 1;
    }
    else if (strcmp(arglist[0], "history") == 0) {
        hist_print();
        return 1;
    }

    return 0;
}
