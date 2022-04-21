/************** test.c file ***************/

#include "ucode.c"

int main(int argc, char *argv[ ]) {
    char cmdline[64];
    int i;
    strcpy(cmdline, argv[0]);
    for (i=2; i<argc; i++) {
        strcat(cmdline, " ");
        strcat(cmdline, argv[i]);
    }

    int status;
    int pid = fe(cmdline);
    pid = wait(&status);
    printf("ZOMBIE PID: %d\n", pid);
    printf("Status: %d\n", status);

    exit(0);
}

// NEED TO RECOMPILE FE TO TEST NEW KFE CODE!!!!!!!!!!!!!!1
