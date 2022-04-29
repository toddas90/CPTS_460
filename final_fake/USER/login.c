#include "ucode.c"

/*
root:12345:0:0:super user:/:sh
kcw:abcde:1:1:kcw user:/user/kcw:sh
guest:mercy:2:2:guest user:/user/guest:sh
buster:123:3:3:buster dog:/user/buster:sh
*/

int in, out, err;
char *tok = ":\n";
char user[32], pass[32], uid[8], gid[8], dir[32], shell[32];

int main(int argc, char *argv[]) {
    close(0); close(1); close(2);

    char name[32], password[32];

    in = open(argv[1], O_RDONLY);
    out = open(argv[1], O_WRONLY);
    err = open(argv[1], O_RDWR);

    settty(argv[1]);

    while (1) {
        printf("login: ");
        gets(name);
        printf("password: ");
        gets(password);

        if (validate_user(name, password)) {
            printf("Welcome %s\n", name);
            chuid(atoi(uid), atoi(gid));
            chdir(dir);
            exec(shell);
        } else {
            printf("Invalid user or password\n");
        }
    }
}

int validate_user(char *name, char *password) {
    char buf[BLKSIZE];
    int fd = open("/etc/passwd", O_RDONLY);
    int i = 0, found = 0;

    if (fd < 0) {
        printf("cannot open /etc/passwd\n");
        return -1;
    }

    int n = read(fd, buf, BLKSIZE);

    while (i < n) {
        strcpy(user, strtok(buf + i, tok));
        strcpy(pass, strtok(NULL, tok));
        strcpy(uid, strtok(NULL, tok));
        strcpy(gid, strtok(NULL, tok));
        strtok(NULL, tok);
        strcpy(dir, strtok(NULL, tok));
        strcpy(shell, strtok(NULL, tok));

        if (strcmp(user, name) == 0) {
            if (strcmp(pass, password) == 0) {
                found = 1;
                break;
            }
        }

        i += strlen(buf + i) + 1;
    }

    close(fd);
    return found;
}
