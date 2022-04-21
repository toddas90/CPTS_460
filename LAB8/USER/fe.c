/************** test.c file ***************/

#include "ucode.c"

int main(int argc, char *argv[ ]) {
    char cmdline[64];
    int i;

    // Copy argv to cmdline, ignoring the first arg (the name of the program)
    strcpy(cmdline, argv[1]);
    for (i=2; i<argc; i++) {
        strcat(cmdline, " ");
        strcat(cmdline, argv[i]);
    }

    printf("cmdline=%s\n", cmdline);

    int status;
    int pid = fe(cmdline);
    pid = wait(&status);
    printf("ZOMBIE PID: %d\n", pid);
    printf("Status: %d\n", status);

    exit(0);
}
