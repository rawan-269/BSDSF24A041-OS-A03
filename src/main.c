#include "shell.h"

int main() {
    char* cmdline;
    char** arglist;

    while ((cmdline = read_cmd(PROMPT, stdin)) != NULL) {
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
            hist_add(cmdline);

            if (!handle_builtin(arglist)) {
                execute(arglist);
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
