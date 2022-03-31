#include "ucode.c"

int main(int argc, char *argv[]) {
    int pid, status;
    char cmdline[64];
    char cmd[1];
    while (1) {
        uprintf("commands=(u1|u2|exit [parameters])\n");
        ugetline(cmdline);
        if (!strcmp(cmdline, "exit")) {
            uexit(0);
        }

        // set cmd to the first two characters of cmdline
        cmd[0] = cmdline[0];
        cmd[1] = cmdline[1];

        if (!strcmp(cmd, "u1") || !strcmp(cmd, "u2")) {
            // uprintf("Hello from %s\n", cmd);
        } else {
            uprintf("unknown command\n");
            continue;
        }

        pid = ufork();
        if (pid) {
            pid = uwait(&status);
        } else {
            exec(cmdline);
        }
    }
}