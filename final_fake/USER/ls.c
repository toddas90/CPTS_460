#include "ucode.c"

int main(int argc, char *argv[]) {
    char path[64];

    if (argc < 2) {
        strcpy(path, "."); // default to current directory
    } else {
        strcpy(path, argv[1]); // use the path specified
    }

    // Open directory in path.
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        printf("ls: cannot open %s\n", path);
        return -1;
    }

    // Code from 360 for traversing directory.
    // ---------------------------------------
    char buf[BLKSIZE];
    int n = read(fd, buf, BLKSIZE);
    DIR *dp;
    char *cp;

    dp = (DIR *)buf;
    cp = buf + dp->rec_len;

    while (cp < buf + n) {
        ls_file(dp->name);
        cp += dp->rec_len;
        dp = (DIR *)cp;
    }
    // ---------------------------------------

    // Close directory.
    close(fd);

    return 0;
}

int ls_file(char *file) {
    char t1[16] = "xwrxwrxwr-------"; 
    char t2[16] = "----------------";

    char name[64];
    strcpy(name, file);

    struct stat st;
    stat(file, &st);

    if (S_ISREG(st.st_mode)) // Check if file
        putc('-');
    if (S_ISDIR(st.st_mode)) // Check if dir
        putc('d');
    if (S_ISLNK(st.st_mode)) // Check if link
        putc('l');
        
    for (int i = 8; i >= 0; i--) { // Print out permissions
        if (st.st_mode & (1 << i)) {
            putc(t1[i]);
        }
        else {
            putc(t2[i]);
        }
    }

    printf(" %d ", st.st_nlink); // Print link count 2

    printf("%d ", st.st_uid); // uid -4

    printf("%d", st.st_gid); // gid -4

    printf("%d ", st.st_size); // Print file size 6
    
    printf("%s\n", name); // print name normally

    return 0;
}
