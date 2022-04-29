/************** sh.c file ***************/

#include "ucode.c"

void do_command(char *cmdline) {
    char head[64];
    strcpy(head, cmdline); // Copy cmdline to head.
    int in, out;
    char *in_file, *out_file;
    in_file = out_file = NULL;
    in = out = 0;
    char tok[6] = "\t\r\n ";

    // Find I/O redirection symbols
    char *token = strtok(cmdline, tok);

    while (token != NULL) {
        if (strcmp(token, "<") == 0) { // Redirect input
            in_file = strtok(NULL, tok);
            in = open(in_file, O_RDONLY);
        } else if (strcmp(token, ">") == 0) { // Truncate output or create.
            out_file = strtok(NULL, tok);
            out = open(out_file, O_WRONLY | O_CREAT | O_TRUNC);
        } else if (strcmp(token, ">>") == 0) { // Append output or create.
            out_file = strtok(NULL, tok);
            out = open(out_file, O_WRONLY | O_CREAT | O_APPEND);
        }
        token = strtok(NULL, tok);
    }

    if (in) { // Redirect input
        close(0);
        dup(in);
        close(in);
    }

    if (out) { // Redirect output
        close(1);
        dup(out);
        close(out);
    }

    exec(head); // Execute command.
}

int scan(char *cmdline, char *head, char *tail) {
    int split = 0;
    int i = 0;

    // Find the rightmost pipe.
    while (cmdline[i] != '\0') {
        if (cmdline[i] == '|') {
            split = i;
        }
        i++;
    }

    // If no pipe, put everything in head and return FALSE.
    if (split == 0) {
        strcpy(head, cmdline);
        strcpy(tail, "");
        return 0;
    } else { // Otherwise, put everything before the pipe in head and everything after in tail. Return TRUE.
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

    // If input is a pipe, close the read end.
    if (pipefd) {
        close(pipefd[0]);
        dup2(pipefd[1], 1);
    }

    // Check if there is a pipe in the command.
    hasPipe = scan(cmdline, head, tail);

    if (hasPipe) {
        // Create a pipe and fork.
        pipe(lpd);
        pid = fork();

        if (pid) {
            // Parent: Close write end of pipe.
            close(lpd[1]);
            dup2(lpd[0], 0);
            do_command(tail);
        } else {
            // Child: Recursively call do_pipe.
            do_pipe(head, lpd);
        }
    } else {
        // Run the command.
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

        // Fork program.
        pid = fork();
        if (pid) {
            pid = wait(&status);
        } else {
            do_pipe(cpy, 0);
        }
    }
}
