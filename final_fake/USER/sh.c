/************** sh.c file ***************/

#include "ucode.c"

int do_pipe(char *cmdline) {
    char *cmd1 = strtok(cmdline, "|");
    char *cmd2 = strtok(NULL, "\n");

    int fd[2];
    pipe(fd);

    int pid = fork();
    if (pid == 0) {
        close(fd[0]);
        dup2(fd[1], 1);
        exec(cmd1);
    } else {
        close(fd[1]);
        dup2(fd[0], 0);
        exec(cmd2);
    }
    return 0;
}

int main(int argc, char *argv[ ])
{
    // pid and status.
    int pid, status;

    // The command to be executed. Checks for trivial cases and handles them if necessary.
    char *cmd;

    // The whole command line entered by the user.
    char cmdline[128], cpy[128];

    // Loop forever.
    while (1) {
        printf("sh> ");
        gets(cmdline);

        strcpy(cpy, cmdline);

        // Checks for pipe in the command line.
        if (strstr(cmdline, "|")) {
            do_pipe(cmdline); // BROKEN PIPE
            continue;
        }

        cmd = strtok(cmdline, " ");
        if (cmd == NULL) {
            continue;
        }

        // Trivial case exit.
        if (strcmp(cmd, "exit") == 0) {
            printf("Exiting shell\n");
            exit(0);
        }

        // Trivial case cd.
        if (strcmp(cmd, "cd") == 0) {
            char *path = strtok(NULL, " ");
            if (path == NULL) {
                chdir("/");
            } else {
                chdir(path);
            }
            continue;
        }

        pid = fork();
        if (pid == 0) {
            exec(cpy);
        } else {
            wait(&status);
        }
    }
}
