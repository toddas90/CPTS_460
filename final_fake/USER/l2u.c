#include "ucode.c"

int main(int argc, char *argv[]) {
    // Take in two files argv[1] and argv[2]. Open them.
    int fd1, fd2, n;
    char buf[BLKSIZE];

    if (argc < 3) {
        fd1 = 0; // Use stdin.
        fd2 = 1; // Use stdout.
    } else {
        fd1 = open(argv[1], O_RDONLY); // Open file.
        fd2 = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC); // Open file.
    }

    if (fd1 < 0) { // Bad file.
        printf("cat: cannot open %s\n", argv[1]);
        return -1;
    }

    if (fd2 < 0) { // Bad file.
        printf("cat: cannot open %s\n", argv[2]);
        return -1;
    }

    // Read from fd1 into buf.
    n = read(fd1, buf, BLKSIZE);

    // Make the buf upper case.
    int i;
    for (i = 0; i < BLKSIZE; i++) {
        if (buf[i] >= 'a' && buf[i] <= 'z') {
            buf[i] -= 32;
        }
    }

    // While there are still bytes in buf, write them to fd2.
    write(fd2, buf, n);

    // Close the files.
    close(fd1);
    close(fd2);

    return 0;
}