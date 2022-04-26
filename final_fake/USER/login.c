#include "ucode.c"

int in, out, err;
// char name[128], password[128];

int main(int argc, char *argv[]) {
    //int in, out, err;
    char name[128], password[128];

    // Close in and out file descriptors.
    close(in);
    close(out);

    in = open(argv[1], O_RDONLY);
    out = open(argv[1], O_WRONLY);
    err = open(argv[1], O_WRONLY);

    settty(argv[1]);

    int fd = open("/etc/passwd", O_RDONLY);

    /*
    root:12345:0:0:super user:/:sh
    kcw:abcde:1:1:kcw user:/user/kcw:sh
    guest:mercy:2:2:guest user:/user/guest:sh
    buster:123:3:3:buster dog:/user/buster:sh
    */

    while (1) {
        printf("Username: ");
        gets(name);
        printf("Password: ");
        gets(password);

        int found = 0;
        char line[128];
        int i = 0;
        while (read(fd, line, 128) > 0) {
            if (strstr(line, name)) {
                found = 1;
                break;
            }
        }

        if (found) {
            if (strstr(line, password)) {
                printf("Login Successful\n");
                char *uid, *gid, *home, *shell, *temp;
                temp = strtok(line, ":");
                temp = strtok(NULL, ":");
                uid = strtok(NULL, ":");
                gid = strtok(NULL, ":");
                temp = strtok(NULL, ":");
                home = strtok(NULL, ":");
                shell = strtok(NULL, "\n");
                // printf("uid: %s\ngid: %s\nhome: %s\nshell: %s\n", uid, gid, home, shell);
                chuid(atoi(uid), atoi(gid));
                chdir(home);
                close(fd);
                exec(shell);
                break;
            } else {
                printf("Bad Login\n");
            }
        } else {
            printf("Bad Login\n");
        }
    }
}

