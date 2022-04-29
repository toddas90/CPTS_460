#include "ucode.c"

int main(int argc, char *argv[]) {
    int fd, n;
    char buf[512];
    char buf2[80];
    char end;

    if (argc < 2) {
        fd = 0; // Use stdin.
    } else {
        fd = open(argv[1], O_RDONLY); // Open file.
    }

    if (fd < 0) { // Bad file.
        printf("cat: cannot open %s\n", argv[1]);
        return -1;
    }

    int temp = 0;

    read(fd, buf, 512); // Read file into buf.
    printf("%s", buf); // Print file.

    // Read line by line
    while (n = readline(fd, buf2, 80)) {
        if (n < 0) {
            printf("cat: cannot read %s\n", argv[1]);
            return -1;
        }
        printf("%s", buf2); // Print
        end = getc(); // Get user input
        putc('\n');
        putc('\r');
        if (end == 'q') {
            break;
        } else if (end == '\r' || end == '\n') {
            printf("\n");
        } else if (end == ' ') {
            while (n = readline(fd, buf2, 80) && temp < 25) {
                printf("%s\n", buf2); // Print
                temp ++;
            }
        }
    }

    close(fd);
    return 0;
}