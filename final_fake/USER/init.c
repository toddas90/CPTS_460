/************** init.c file ***************/

#include "ucode.c"

int console[3];
char ttys[3][16] = {"/dev/tty0", "/dev/ttyS0", "/dev/ttyS1"};

void main() {
    int i;
    int in[3], out[3];
    int pid, status;

    for (i = 0; i < 3; i++) {
        in[i] = open(ttys[i], O_RDONLY);
        out[i] = open(ttys[i], O_WRONLY);
        console[i] = fork();
        if (console[i] == 0) {
            exec(strcat("login ", ttys[i]));
        }
    }
    
    while (1) {
        pid = wait(&status);
        
        for (int i = 0; i < 3; i++) {
            if (pid == console[i]) {
                printf("Forking new console process\n");
                console[i] = fork();
                if (console[i]) {
                    continue;
                } else {
                    exec(strcat("login ", ttys[i]));
                }
                printf("Buried process %d\n", pid);
            }
        }
    }
}

// int console;

// int parent() {
//     int pid, status;

//     while (1) {
//         pid = wait(&status);
        
//         if (pid == console) {
//             printf("Forking new console process\n");
//             console = fork();
//             if (console) {
//                 continue;
//             } else {
//                 exec("login /dev/tty0");
//             }
//             printf("Buried process %d\n", pid);
//         }
//     }
// }

// void main() {
//     int in, out;
//     in = open("/dev/tty0", O_RDONLY);
//     out = open("/dev/tty0", O_WRONLY);
//     printf("Forking login process\n");

//     console = fork();

//     if (console) {
//         parent();
//     } else {
//         exec("login /dev/tty0");
//     }
// }