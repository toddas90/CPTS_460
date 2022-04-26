/************** sh.c file ***************/

#include "ucode.c"

int do_pipe(char *cmdline) {
    char *cmd1, *cmd2;
    int pd[2];
    int pid;

    cmd1 = strtok(cmdline, "|");
    cmd2 = strtok(NULL, "|");

    if (cmd1 == NULL) {
        return 0;
    }

    if (cmd2 == NULL) {
        exec(cmd1);
        return 0;
    }

    pipe(pd);
    pid = fork();

    if (pid == 0) {
        close(pd[0]);
        dup2(pd[1], 1);
        do_pipe(cmd2);
    } else {
        close(pd[1]);
        dup2(pd[0], 0);
        do_pipe(cmd1);
    }
}

int main(int argc, char *argv[ ])
{
    // pid and status.
    int pid, status;

    // The command to be executed. Checks for trivial cases and handles them if necessary.
    char *cmd;

    // The whole command line entered by the user and a copy.
    char *cmdline, *execute;

    // Loop forever.
    while (1) {
        printf("sh> ");
        gets(cmdline);

        strcpy(execute, cmdline);

        cmd = strtok(cmdline, " ");
        if (cmd == NULL) {
            continue;
        }

        // // Checks for pipe in the command line.
        // if (strstr(cmdline, "|")) {
        //     do_pipe(cmdline);
        //     continue;
        // }

        // Trivial case exit.
        if (strcmp(cmd, "exit") == 0) {
            printf("Exiting shell\n");
            exit(0);
        }

        pid = fork();
        if (pid == 0) {
            exec(execute);
        } else {
            wait(&status);
        }
    }
}
