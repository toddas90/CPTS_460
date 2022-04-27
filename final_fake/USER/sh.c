/************** sh.c file ***************/

#include "ucode.c"

void do_command(char *cmdline) {
    // printf("-----Exec-----\n");
    // printf("cmdline: %s\n", cmdline);
    // printf("--------------\n");

    exec(cmdline);
}

int scan(char *cmdline, char *head, char *tail) {
    int split = 0;
    int i = 0;

    while (cmdline[i] != '\0') {
        if (cmdline[i] == '|') {
            split = i;
        }
        i++;
    }

    if (split == 0) {
        strcpy(head, cmdline);
        strcpy(tail, "");
        return 0;
    } else {
        strncpy(head, cmdline, split);
        head[split - 1] = '\0';
        strcpy(tail, cmdline + split + 2);
        return 1;
    }
}

void do_pipe(char *cmdline, int *pipefd) {
    int lpd[2];
    int pid, hasPipe;
    char head[64], tail[64];

    if (pipefd) {
        close(pipefd[0]);
        dup2(pipefd[1], 1);
        // close(pipefd[1]);
    }

    hasPipe = scan(cmdline, head, tail);

    // printf("----------do_pipe----------\n");
    // printf("head: %s\n", head);
    // printf("tail: %s\n", tail);
    // printf("cmdline: %s\n", cmdline);
    // printf("hasPipe: %d\n", hasPipe);
    // printf("---------------------------\n");

    if (hasPipe) {
        pipe(lpd);
        pid = fork();

        if (pid) {
            close(lpd[1]);
            dup2(lpd[0], 0);
            // close(lpd[0]);
            do_command(tail);
        } else {
            do_pipe(head, lpd);
        }
    } else {
        do_command(cmdline);
    }
}

int main(int argc, char *argv[ ])
{
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
        if (pid) {
            pid = wait(&status);
        } else {
            do_pipe(cpy, 0);
        }
    }
}
