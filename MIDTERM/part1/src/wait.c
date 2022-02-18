int tswitch();

int delete(PROC *p) {
    if (p->parent->child == p) {
        if (p->sibling == NULL) {
            p->parent->child = NULL;
        } else {
            p->parent->child = p->sibling;
            p->sibling = NULL;
        }
        return 0;
    }

    if (p->parent->child != p) {
        PROC *j = p->parent->child;
        while (j->sibling != p)
            j = j->sibling;
        if (p->sibling != NULL) {
            j->sibling = p->sibling;
            p->sibling = NULL;
        } else {
            j->sibling = NULL;
        }
        return 0;
    }
}

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
      p->priority = 0;
      delete(p);
      //p->sibling = NULL;
      //p->child = NULL;
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
  enqueue(&sleepList, running);
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
      dequeue(&sleepList);
      enqueue(&readyQueue, p);
    }
  }
  int_on(SR);
}

int kexit(int exitValue) {
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


  
