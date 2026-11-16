#include "shell.h"
#include <readline/readline.h>
#include <readline/history.h>

/* Execute one command segment */
/* Feature 4: Command completion */
static char *command_generator(const char *text, int state) {
    static const char *commands[] = {
        "cd", "exit", "help", "jobs", "history", "set",
        "echo", "pwd", "ls", "cat", "grep", "mkdir",
        "rmdir", "rm", "touch", "cp", "mv", "clear",
        "date", "whoami", "uname", "head", "tail",
        "wc", "sort", "sleep", "true", "false",
        NULL
    };

    static int index;
    static size_t length;

    if (state == 0) {
        index = 0;
        length = strlen(text);
    }

    while (commands[index] != NULL) {
        const char *name = commands[index++];

        if (strncmp(name, text, length) == 0) {
            return strdup(name);
        }
    }

    return NULL;
}

static char **shell_completion(
    const char *text, int start, int end
) {
    (void)end;

    /* Complete command names only at the beginning */
    if (start != 0) {
        return NULL;
    }

    rl_attempted_completion_over = 0;

    return rl_completion_matches(text, command_generator);
}
static int run_segment(char** seg, int background) {

    char* label = join_tokens(seg);

    /* Handle variable assignment as a built-in */
    if (seg[0] != NULL && seg[1] == NULL &&
        is_assignment(seg[0])) {

        char* equal = strchr(seg[0], '=');
        *equal = '\0';

        set_variable(seg[0], equal + 1);

        *equal = '=';

        free(label);
        return 0;
    }

    Command cmds[MAXCMDS];

    int n = parse_commands(seg, cmds);
    int exit_status = 0;

    if (n > 0) {

        int simple = (n == 1 &&
                      cmds[0].infile == NULL &&
                      cmds[0].outfile == NULL);

        if (simple && handle_builtin(cmds[0].argv)) {
            exit_status = 0;
        }
        else {

            pid_t pid = execute_pipeline(
                cmds, n, background, &exit_status
            );

            if (background && pid > 0) {
                job_add(pid, label);
                label = NULL;
            }

            if (background) {
                exit_status = 0;
            }
        }
    }
    else {
        exit_status = 1;
    }

    free(label);

    return exit_status;
}


/* Execute a complete command line */

static int process_line(char* line) {

    char** arglist = tokenize(line);

    if (arglist == NULL) {
        return 0;
    }

    add_history(line);
    hist_add(line);

    int i = 0;
    int last_status = 0;

    while (arglist[i] != NULL) {

        int start_index = i;

        char* seg[MAXARGS + 1];
        int k = 0;

        while (arglist[i] != NULL &&
               strcmp(arglist[i], ";") != 0 &&
               strcmp(arglist[i], "&") != 0) {

            seg[k++] = arglist[i++];
        }

        seg[k] = NULL;

        /* Expand variables for this segment */
        expand_variables(seg);

        /* Update original pointers to avoid double-free */
        for (int j = 0; j < k; j++) {
            arglist[start_index + j] = seg[j];
        }

        int background = 0;

        if (arglist[i] != NULL) {
            background = (strcmp(arglist[i], "&") == 0);
            i++;
        }

        if (k == 0) {
            fprintf(stderr, "syntax error near '%s'\n",
                    background ? "&" : ";");
            last_status = 1;
            break;
        }

        last_status = run_segment(seg, background);
    }

    for (int j = 0; arglist[j] != NULL; j++) {
        free(arglist[j]);
    }

    free(arglist);

    return last_status;
}


/* Remove spaces from beginning and end */

static char* trim_line(char* line) {

    while (*line == ' ' || *line == '\t') {
        line++;
    }

    int len = strlen(line);

    while (len > 0 &&
           (line[len - 1] == ' ' ||
            line[len - 1] == '\t' ||
            line[len - 1] == '\n')) {

        line[--len] = '\0';
    }

    return line;
}


/* Execute commands stored in a block */

static void execute_block(char* commands[], int count) {

    for (int i = 0; i < count; i++) {
        process_line(commands[i]);
    }
}


/* Feature 7: if-then-else-fi */

static void handle_if(char* condition) {

    char* then_commands[100];
    char* else_commands[100];

    int then_count = 0;
    int else_count = 0;

    int in_else = 0;
    int found_then = 0;
    int found_fi = 0;

    char* line;

    while (1) {

        line = readline("...> ");

        if (line == NULL) {
            break;
        }

        char* trimmed = trim_line(line);

        if (strcmp(trimmed, "then") == 0) {
            found_then = 1;
            free(line);
            continue;
        }

        if (strcmp(trimmed, "else") == 0) {

            if (!found_then) {
                fprintf(stderr, "syntax error: expected then\n");
                free(line);
                break;
            }

            in_else = 1;
            free(line);
            continue;
        }

        if (strcmp(trimmed, "fi") == 0) {
            found_fi = 1;
            free(line);
            break;
        }

        if (!found_then) {
            fprintf(stderr, "syntax error: expected then\n");
            free(line);
            break;
        }

        if (!in_else) {

            if (then_count < 100) {
                then_commands[then_count++] = strdup(trimmed);
            }
            else {
                fprintf(stderr, "too many then commands\n");
                free(line);
                break;
            }
        }
        else {

            if (else_count < 100) {
                else_commands[else_count++] = strdup(trimmed);
            }
            else {
                fprintf(stderr, "too many else commands\n");
                free(line);
                break;
            }
        }

        free(line);
    }

    if (!found_then || !found_fi) {
        fprintf(stderr, "syntax error in if statement\n");
        goto cleanup;
    }

    int condition_status = process_line(condition);

    if (condition_status == 0) {
        execute_block(then_commands, then_count);
    }
    else {
        execute_block(else_commands, else_count);
    }

cleanup:

    for (int i = 0; i < then_count; i++) {
        free(then_commands[i]);
    }

    for (int i = 0; i < else_count; i++) {
        free(else_commands[i]);
    }
}


/* Main function */

int main() {
        rl_attempted_completion_function = shell_completion;
    char* cmdline;

    while (1) {

        reap_jobs();

        cmdline = readline(PROMPT);

        if (cmdline == NULL) {
            break;
        }

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

        char* trimmed = trim_line(cmdline);

        if (strncmp(trimmed, "if ", 3) == 0) {

            char* condition = strdup(trimmed + 3);

            handle_if(condition);

            free(condition);
        }
        else {
            process_line(trimmed);
        }

        free(cmdline);
    }

    printf("\nShell exited.\n");

    return 0;
}
