/************** sh.c file ***************/

#include "ucode.c"

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
        
        if (strcmp(cmd, "exit") == 0) { // Trivial case: exit.
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
