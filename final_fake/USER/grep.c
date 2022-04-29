#include "ucode.c"

int main(int argc, char *argv[]) {
    int fd;
    char buf[BLKSIZE];
    char pattern[32];

    if (argc < 2) {
        printf("Usage: grep <pattern> <file>\n");
        return -1;
    }

    strcpy(pattern, argv[1]);

    if (argc < 3) {
        fd = 0; // Use stdin.
    } else {
        fd = open(argv[2], O_RDONLY); // Open file.
    }

    if (fd < 0) { // Bad file.
        printf("cat: cannot open %s\n", argv[2]);
        return -1;
    }

    // Print lines of the file that contain the string argv[1].
    while (readline(fd, buf, BLKSIZE)) {
        if (strstr(buf, pattern)) {
            printf("%s\n", buf);
        }

        // Check for EOF.
        if (strcmp(buf, "\n") == 0 || strcmp(buf, "\0") == 0) {
            printf("Stuck\n");
            break;
        }
    }

    close(fd);
    return 0;
}