#include "shell.h"
#include <readline/readline.h>
#include <readline/history.h>

static void run_segment(char** seg, int background) {
    char* label = join_tokens(seg);
    Command cmds[MAXCMDS];
    int n = parse_commands(seg, cmds);

    if (n > 0) {
        int simple = (n == 1 && cmds[0].infile == NULL && cmds[0].outfile == NULL);
        if (!(simple && handle_builtin(cmds[0].argv))) {
            pid_t pid = execute_pipeline(cmds, n, background);
            if (background && pid > 0) {
                job_add(pid, label);
                label = NULL;
            }
        }
    }

    free(label);
}

int main() {
    char* cmdline;
    char** arglist;

    while (1) {
        reap_jobs();

        cmdline = readline(PROMPT);
        if (cmdline == NULL) break;

        if (cmdline[0] == '\0') {
            free(cmdline);
            continue;
        }

        if (cmdline[0] == '!') {
            int n = atoi(cmdline + 1);
            char* h = hist_get(n);
            if (h == NULL) {
                printf("history: event not found\n");
                free(cmdline);
                continue;
            }
            free(cmdline);
            cmdline = strdup(h);
            printf("%s\n", cmdline);
        }

        if ((arglist = tokenize(cmdline)) != NULL) {
            add_history(cmdline);
            hist_add(cmdline);

            int i = 0;
            while (arglist[i] != NULL) {
                char* seg[MAXARGS + 1];
                int k = 0;

                while (arglist[i] != NULL &&
                       strcmp(arglist[i], ";") != 0 &&
                       strcmp(arglist[i], "&") != 0) {
                    seg[k++] = arglist[i++];
                }
                seg[k] = NULL;

                int background = 0;
                if (arglist[i] != NULL) {
                    background = (strcmp(arglist[i], "&") == 0);
                    i++;
                }

                if (k == 0) {
                    fprintf(stderr, "syntax error near '%s'\n", background ? "&" : ";");
                    break;
                }

                run_segment(seg, background);
            }

            for (int j = 0; arglist[j] != NULL; j++) {
                free(arglist[j]);
            }
            free(arglist);
        }
        free(cmdline);
    }

    printf("\nShell exited.\n");
    return 0;
}
