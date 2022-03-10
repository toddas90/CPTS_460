#include "type.h"

extern PROC *running;
extern PROC *readyQueue;
extern PROC *sleepList;
extern PROC *freeList;
extern PROC proc[NPROC];

int kwait(int *status) {
    PROC *p;
    int found = 0;

    for (int i = 0; i < NPROC; i++) {
        if (proc[i].ppid == running->pid) {
            found = 1;
            break;
        }
    }

    if (found == 0) {
        return -1;
    }

    while (1) {
        for (int i = 0; i < NPROC; i++) {
            if (proc[i].ppid == running->pid) {
                p = &proc[i];
                break;
            }
        }

        if (p) {
            int cpid = p->pid;
            *status = p->exitCode;
            p->status = FREE;
            enqueue(&freeList, p);
            return cpid;
        }
        ksleep(running);
    }
}

int kwakeup(int event) {
  // Turn off interrupts.
  // loop through procs and wake any up who match the event
  int SR = int_off();
  PROC *p;
  for (int i = 1; i < NPROC; i++) {
    p = &proc[i];
    if (p->status == SLEEP && p->event == event) {
      p->status = READY;
      dequeue(&sleepList);
      enqueue(&readyQueue, p);
      //do_ps();
    }
  }
  int_on(SR);
}

int ksleep(int event) {
  // This needs to be atomic, so disable interrupts
  // record event value in running proc.
  // change running proc status to sleep.
  // switch processes.
  int SR = int_off();
  running->event = event;
  running->status = SLEEP;
  enqueue(&sleepList, running);
  tswitch();
  int_on(SR);
}

int kexit(int exitValue) {
  printf("proc %d in kexit(), value=%d\n", running->pid, exitValue);

  running->exitCode = exitValue;
  running->status = ZOMBIE;
  running->priority = 0;

  kwakeup(running->parent);
  tswitch();
}



