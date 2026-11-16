#include "shell.h"

pid_t execute_pipeline(Command* cmds, int n, int background,
                       int* exit_status) {

    int prev_read = -1;
    int fd[2];
    pid_t pids[MAXCMDS];

    int started = 0;
    int status = 1;

    if (exit_status != NULL) {
        *exit_status = 1;
    }

    for (int i = 0; i < n; i++) {

        if (i < n - 1 && pipe(fd) < 0) {
            perror("pipe failed");
            break;
        }

        pid_t pid = fork();

        if (pid < 0) {
            perror("fork failed");

            if (i < n - 1) {
                close(fd[0]);
                close(fd[1]);
            }

            break;
        }

        if (pid == 0) {

            if (prev_read != -1) {
                dup2(prev_read, STDIN_FILENO);
                close(prev_read);
            }

            if (i < n - 1) {
                close(fd[0]);
                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);
            }

            if (background && i == 0 &&
                cmds[i].infile == NULL) {

                int f = open("/dev/null", O_RDONLY);

                if (f >= 0) {
                    dup2(f, STDIN_FILENO);
                    close(f);
                }
            }

            if (cmds[i].infile != NULL) {

                int f = open(cmds[i].infile, O_RDONLY);

                if (f < 0) {
                    perror(cmds[i].infile);
                    exit(1);
                }

                dup2(f, STDIN_FILENO);
                close(f);
            }

            if (cmds[i].outfile != NULL) {

                int f = open(cmds[i].outfile,
                             O_WRONLY | O_CREAT | O_TRUNC,
                             0644);

                if (f < 0) {
                    perror(cmds[i].outfile);
                    exit(1);
                }

                dup2(f, STDOUT_FILENO);
                close(f);
            }

            execvp(cmds[i].argv[0], cmds[i].argv);

            perror("Command not found");
            exit(1);
        }

        pids[started++] = pid;

        if (prev_read != -1) {
            close(prev_read);
            prev_read = -1;
        }

        if (i < n - 1) {
            close(fd[1]);
            prev_read = fd[0];
        }
    }

    if (prev_read != -1) {
        close(prev_read);
    }

    if (background) {

        if (started == n) {
            return pids[started - 1];
        }

        return 0;
    }

    for (int i = 0; i < started; i++) {

        int wait_result;

        do {
            wait_result = waitpid(pids[i], &status, 0);
        } while (wait_result < 0 && errno == EINTR);

        if (wait_result < 0) {
            perror("waitpid failed");
            continue;
        }

        if (i == started - 1 && exit_status != NULL) {

            if (WIFEXITED(status)) {
                *exit_status = WEXITSTATUS(status);
            }
            else if (WIFSIGNALED(status)) {
                *exit_status = 128 + WTERMSIG(status);
            }
        }
    }

    return 0;
}
