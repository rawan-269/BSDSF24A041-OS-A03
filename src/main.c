#include "shell.h"
#include <readline/readline.h>
#include <readline/history.h>

int main() {
    char* cmdline;
    char** arglist;

    while ((cmdline = readline(PROMPT)) != NULL) {
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

            Command cmds[MAXCMDS];
            int n = parse_commands(arglist, cmds);

            if (n > 0) {
                int simple = (n == 1 && cmds[0].infile == NULL && cmds[0].outfile == NULL);
                if (!(simple && handle_builtin(cmds[0].argv))) {
                    execute_pipeline(cmds, n);
                }
            }

            for (int i = 0; arglist[i] != NULL; i++) {
                free(arglist[i]);
            }
            free(arglist);
        }
        free(cmdline);
    }

    printf("\nShell exited.\n");
    return 0;
}
