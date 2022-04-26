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

    while (1) {
        printf("Login: ");
        gets(name);
        printf("Password: ");
        gets(password);

        char line[128];
        int n = read(fd, line, 128);
        if (n == 0) {
            break;
        }

        /*
        root:12345:0:0:super user:/:sh
        kcw:abcde:1:1:kcw user:/user/kcw:sh
        guest:mercy:2:2:guest user:/user/guest:sh
        buster:123:3:3:buster dog:/user/buster:sh
        */

        char *name_from_file = strtok(line, ":");
        char *password_from_file = strtok(NULL, ":");
        char *uid_from_file = strtok(NULL, ":");
        char *gid_from_file = strtok(NULL, ":");
        char *comment_from_file = strtok(NULL, ":");
        char *home_from_file = strtok(NULL, ":");
        char *shell_from_file = strtok(NULL, "\n");

        // If the name and password match, then we are done.
        if (strcmp(name, name_from_file) == 0 && strcmp(password, password_from_file) == 0) {
            printf("Login successful!\n");
            printf("user: %s\n", name_from_file);
            printf("home: %s\n", home_from_file);
            printf("shell: %s\n", shell_from_file);
            chuid(atoi(uid_from_file), atoi(gid_from_file));
            chdir(home_from_file);
            close(fd);
            exec(shell_from_file);
        }

        // If the name and password don't match, print an error message.
        if (strcmp(name, name_from_file) != 0 || strcmp(password, password_from_file) != 0) {
            printf("Login incorrect!\n");
        }
    }
}

