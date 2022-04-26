/************** init.c file ***************/

#include "ucode.c"

int console;

int parent() {
    int pid, status;

    while (1) {
        pid = wait(&status);
        
        if (pid == console) {
            printf("Forking new console process\n");
            console = fork();
            if (console) {
                continue;
            } else {
                exec("login /dev/tty0");
            }
            printf("Buried process %d\n", pid);
        }
    }
}

void main() {
    int in, out;
    in = open("/dev/tty0", O_RDONLY);
    out = open("/dev/tty0", O_WRONLY);
    printf("Forking login process\n");

    console = fork();

    if (console) {
        parent();
    } else {
        exec("login /dev/tty0");
    }
}