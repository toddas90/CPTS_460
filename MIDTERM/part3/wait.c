// wait.c file: ksleep(), kwakeup(), kexit()
#include "type.h"

extern PROC *running;
extern PROC *readyQueue;
extern PROC *sleepList;
extern PROC *freeList;
extern PROC proc[NPROC];
extern PIPE *kpipe;

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
  if (running->pid == 1) {
    //printf("Sorry, I can't let you do that.\n");
    return -1;
  }
  printf("proc %d in kexit(), value=%d\n", running->pid, exitValue);

  running->exitCode = exitValue;
  running->status = ZOMBIE;
  running->priority = 0;

  if (exitValue == 1) {
      kwakeup(&kpipe->room);
      kpipe->nreader--;
  } else if (exitValue == 2) {
      kpipe->nwriter--;
  }
  kwakeup(running->parent);
  tswitch();
}

// int kexit(int exitCode)
// {
//   running->exitCode = exitCode;
//   running->status = ZOMBIE;
//   running->priority = 0;
//   kwakeup(running->parent);
//   tswitch();
// }
