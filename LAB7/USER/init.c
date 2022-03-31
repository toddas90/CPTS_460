#include "ucode.c"

void main() {
    int sh, pid, status;
    sh = fork();
    if (sh) {
        while (1) {
            pid = wait(&status);
            if (pid == sh) {
                sh = fork();
                if (!sh) {
                    exec("sh start");
                }
                continue;
            }
            uprintf("P1 just buried an orphan %d!\n", pid);
        }
    } else {
        exec("sh start");
    }
}