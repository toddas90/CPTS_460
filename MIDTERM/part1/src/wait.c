int tswitch();

int kwait(int *status) {
  if (running->child == 0) {
    return -1; // Error
  }

  PROC *p;
  while (1) {
    p = running->child;
    while (p) {
      if (p->status == ZOMBIE) {
        break;
      } else {
        p = p->sibling;
      }
    }
    if (p->status == ZOMBIE) {
      int child_pid = p->pid;
      *status = p->exitCode;
      p->status = FREE;
      if (running->child == running) {
        running->child = running->sibling;
      }
      enqueue(&freeList, p);
      return child_pid;
    }
    ksleep(running);
  }
}

int ksleep(int event) {
  // This needs to be atomic, so disable interrupts
  // record event value in running proc.
  // change running proc status to sleep.
  // switch processes.
  int SR = int_off();
  running->event = event;
  running->status = SLEEP;
  tswitch();
  int_on(SR);
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
      enqueue(&readyQueue, p);
    }
  }
  int_on(SR);
}

int kexit(int exitValue) {
  // Dispose of children, if any.
  // record exit value in proc.
  // become a ZOMBIE but don't free it yet,
  // wake up the parent, and p1 if necessary.
  // switch.
  if (running->pid == 1) {
    printf("Sorry, I can't let you do that.\n");
    return -1;
  }
  printf("proc %d in kexit(), value=%d\n", running->pid, exitValue);

  if (running->child) { // If there are children.
    printf("Moving children to grandma's house!\n");
    PROC *p = running->child;
    PROC *q = running->child;
    
    while (q) { // Change all of the children's ppid values to 1.
      q->ppid = 1;
      q->parent = &proc[1];
      q = q->sibling;
    }


    // I THINK THE CHILDLIST BUG IS IN THE FOLLOWING CODE
    PROC *insert = &proc[1];
    insert = insert->child;
     
    if (insert == 0) { // Find where to insert children into 1's tree.
      insert = p;
    } else {
      while (insert->sibling) {
        insert = insert->sibling;
      }
      insert->sibling = p;
    }
  }
  
  running->exitCode = exitValue;
  running->status = ZOMBIE;
  kwakeup(running->parent);
  tswitch();
}


  
