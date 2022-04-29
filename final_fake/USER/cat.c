#include "ucode.c"

int main(int argc, char *argv[]) {
    int fd, n;
    char buf[BLKSIZE];

    if (argc < 2) {
        fd = 0; // Use stdin.
    } else {
        fd = open(argv[1], O_RDONLY); // Open file.
    }

    if (fd < 0) { // Bad file.
        printf("cat: cannot open %s\n", argv[1]);
        return -1;
    }

    struct stat st;
    fstat(1, &st);

    // THIS WILL NOT ECHO OUTPUT BACK TO USER IN CURRENT STATE!!!!

    if (S_ISCHR(st.st_mode)) { // If output is a character device, use printf (it adds \r\n).
        while (n = read(fd, buf, BLKSIZE)) {
            if (n < 0) {
                printf("cat: cannot read %s\n", argv[1]);
                return -1;
            }
            printf("%s", buf);
        }
    } else { // If output is a file, copy it directly using write.
        while (n = read(fd, buf, BLKSIZE)) {
            if (n < 0) {
                printf("cat: cannot read %s\n", argv[1]);
                return -1;
            }
            write(1, buf, n);
        }
    }

    close(fd);
    return 0;
}