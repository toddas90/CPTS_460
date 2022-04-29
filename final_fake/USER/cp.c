#include "ucode.c"

int main(int argc, char *argv[]) {
    char path_src[64];
    char path_dst[64];

    if (argc < 3) {
        printf("Usage: cp <src> <dst>\n");
        return -1;
    }

    strcpy(path_src, argv[1]);
    strcpy(path_dst, argv[2]);

    // Open source file.
    int fd_src = open(path_src, O_RDONLY);
    if (fd_src < 0) {
        printf("cp: cannot open %s\n", path_src);
        return -1;
    }

    // Open destination file.
    int fd_dst = open(path_dst, O_WRONLY | O_CREAT | O_TRUNC);
    if (fd_dst < 0) {
        printf("cp: cannot open %s\n", path_dst);
        return -1;
    }

    // Copy file.
    char buf[BLKSIZE];
    int n = read(fd_src, buf, BLKSIZE);
    while (n > 0) {
        write(fd_dst, buf, n);
        n = read(fd_src, buf, BLKSIZE);
    }

    close(fd_src);
    close(fd_dst);

    return 0;
}