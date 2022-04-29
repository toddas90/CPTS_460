/************** init.c file ***************/

#include "ucode.c"

int console[3];
int in[3], out[3];
char *tty[3] = {"/dev/tty0", "/dev/ttyS0", "/dev/ttyS1"};
char *tty_exec[3] = {"login /dev/tty0", "login /dev/ttyS0", "login /dev/ttyS1"};

// Parent code from book!
int parent() {
    int pid, status;
    int len = sizeof(tty) / sizeof(tty[0]);

    while (1) {
        pid = wait(&status);
        
        for (int i = 0; i < len; i++) {
            if (pid == console[i]) {
                printf("Forking new console process\n");
                console[i] = fork();
                if (console[i]) {
                    continue;
                } else {
                    exec(tty_exec[i]);
                }
                printf("Buried process %d\n", pid);
            }
        }
    }
}

void main() {
    // Fork 3 login processes on tty0, ttyS0, ttyS1.
    // Hard coded for now, unfortunately.
    console[0] = fork();
    if (console[0]) {
        console[1] = fork();
        if (console[1]) {
            console[2] = fork();
            if (console[2]) {
                parent();
            } else {
                exec(tty_exec[2]);
            }
        } else {
            exec(tty_exec[1]);
        }
    } else {
        exec(tty_exec[0]);
    }
}

int exec_tty(int i) {
    // Open tty's and exec.
    in[i] = open(tty[i], O_RDONLY);
    out[i] = open(tty[i], O_WRONLY);
    exec(tty_exec[i]);
}