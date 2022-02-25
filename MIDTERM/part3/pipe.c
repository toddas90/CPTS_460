// pipe.c file

#include "type.h"

#define NPIPE  4
PIPE pipe[NPIPE];

int pipe_init()
{
    for (int i = 0; i < NPIPE; i++) {
        for (int j = 0; j < PSIZE; j++) {
            pipe[i].buf[j] = 0;
        }
        pipe[i].head = 0;
        pipe[i].tail = 0;
        pipe[i].data = 0;
        pipe[i].room = PSIZE;
        pipe[i].nreader = 0;
        pipe[i].nwriter = 0;
        pipe[i].status = FREE;
    }
}

PIPE *create_pipe()
{
    PIPE *new_pipe;
    for (int i = 0; i < NPIPE; i++) {
        if (pipe[i].status == FREE) {
            // Free pipe found
            for (int j = 0; j < PSIZE; j++) {
                pipe[i].buf[j] = 0;
            }
            pipe[i].head = 0;
            pipe[i].tail = 0;
            pipe[i].data = 0;
            pipe[i].room = PSIZE;
            pipe[i].nreader = 1;
            pipe[i].nwriter = 1;
            pipe[i].status = READY;
            *new_pipe = pipe[i];
            return new_pipe;
        }
    }
    printf("No free pipes!\n");
    return 0;
}

int read_pipe(PIPE *p, char *buf, int n)
{
    int r = 0;

    if (n <= 0) {
        return 0;
    }

    if (p->status == FREE) {
        return 0;
    }

    while (n) {
        if (p->data == 0 && p->nwriter == 0) {
            return 0;
        }
        while (p->data) {
            *buf++ = p->buf[p->tail++]; // Read byte into buf
            p->tail %= PSIZE;
            p->data--; p->room++; r++; n--;

            if (n == 0) {
                break;
            }
        }
        kwakeup(&p->room); // Wake up writers
        
        if (r) {
            return r;
        }
        // Pipe has no data
        ksleep(&p->data); // Sleep for data
    }
  // add code to handle: no data AND no writer: return 0
}

int write_pipe(PIPE *p, char *buf, int n)
{
    int r = 0;

    if (n <= 0) {
        return 0;
    }

    if (p->status == FREE) {
        return 0;
    }

    while (n) {
        if (p->nreader == 0) {
            printf("Broken pipe!\n");
            kexit(2);
        }
        while (p->room > 0) {
            p->buf[p->head++] = *buf++; // Write a byte to pipe
            p->head %= PSIZE;
            p->data++; p->room--; r++; n--;

            if (n == 0) {
                break;
            }
        }
        kwakeup(&p->data); // Wake up readers

        if (n == 0) {
            return r; // Finished writing n bytes
        }
        // More data, no more room
        ksleep(&p->room); // Sleep for room
    }
  // add code to detect BROKEN pipe: print BROKEN pipe message, then exit
}

int print_pipe(PIPE *p) {
    color = RED;
    printf("]========== Pipe Info ==========[\n");
    printf("Status = %d, Reader = %d, Writer = %d\n", p->status, p->nreader, p->nwriter);
    printf("Data = %d, Room = %d, Contents = %s\n", p->data, p->room, p->buf);
    printf("]===============================[\n");
    color = RED + running->pid;
}
  
