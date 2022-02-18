#include "type.h"
#include "string.c"

#include "timer.c"

PROC proc[NPROC];      // NPROC PROCs
PROC *freeList;        // freeList of PROCs 
PROC *readyQueue;      // priority queue of READY procs
PROC *running;         // current running proc pointer
TQUE timers[NPROC];      // NPROC Timer queues

PROC *sleepList;       // list of SLEEP procs
int procsize = sizeof(PROC);

#define NULL 0
#define printf kprintf
#define gets kgets

#include "kbd.c"
#include "vid.c"
#include "exceptions.c"

#include "queue.c"
#include "wait.c"      // include wait.c file

/*******************************************************
  kfork() creates a child process; returns child pid.
  When scheduled to run, child PROC resumes to body();
********************************************************/
int body(), tswitch(), do_sleep(), do_wakeup(), do_exit(), do_switch();
int do_kfork(), do_wait();
int scheduler();
void timer_init(); void timer_handler(); void timer_start();

int kprintf(char *fmt, ...);

void copy_vectors(void) {
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;
    while(vectors_src < &vectors_end)
       *vectors_dst++ = *vectors_src++;
}

void IRQ_handler()
{
    int vicstatus, sicstatus;

    // read VIC SIV status registers to find out which interrupt
    vicstatus = VIC_STATUS;
    sicstatus = SIC_STATUS;  
    if (vicstatus & (1<<31)){ // SIC interrupts=bit_31=>KBD at bit 3 
      if (sicstatus & (1<<3)){
          kbd_handler();
       }
    }
    if (vicstatus & 0x0010){   // timer0,1=bit4
        timer_handler(0);
    }
}

// initialize the MT system; create P0 as initial running process
int init() 
{
  int i;
  PROC *p;
  for (i=0; i<NPROC; i++){ // initialize PROCs
    p = &proc[i];
    p->pid = i;            // PID = 0 to NPROC-1  
    p->status = FREE;
    p->priority = 0;      
    p->next = p+1;
    p->child = NULL;
    p->sibling = NULL;
    p->parent = NULL;
  }
  proc[NPROC-1].next = 0;  
  freeList = &proc[0];     // all PROCs in freeList     
  readyQueue = 0;          // readyQueue = empty

  sleepList = 0;           // sleepList = empty
  
  // create P0 as the initial running process
  p = running = dequeue(&freeList); // use proc[0] 
  p->status = READY;
  p->priority = 0;
  p->ppid = 0;             // P0 is its own parent
  p->sibling = NULL; // No siblings
  p->parent = p; // No parent
  p->child = NULL; // No children yet

  do_kfork();
  do_kfork();
  do_kfork();

  printList("freeList", freeList);
  printf("init complete: P0 running\n"); 
}

int menu()
{
  printf("***************************************\n");
  printf(" ps fork switch exit sleep wakeup wait \n");
  printf("***************************************\n");
}

char *status[ ] = {"FREE", "READY", "SLEEP", "ZOMBIE"};

int do_ps()
{
  int i;
  PROC *p;
  printf("PID  PPID  status\n");
  printf("---  ----  ------\n");
  for (i=0; i<NPROC; i++){
    p = &proc[i];
    printf(" %d    %d    ", p->pid, p->ppid);
    if (p == running)
      printf("RUNNING\n");
    else
      printf("%s\n", status[p->status]);
  }
}
    
int body()   // process body function
{
  int c;
  char cmd[64];

  printf("proc %d starts from body()\n", running->pid);
  while(1){
    printf("***************************************\n");
    printf("proc %d running: parent=%d\n", running->pid,running->ppid);
    printChildList(running);
    printList("freeList", freeList);
    printList("readyQueue", readyQueue);
    printSleepList(sleepList);
    //menu();
    printf("enter a time (in seconds) to wait : ");
    printf("\n");

    gets(cmd);

    timers[running->pid].process = &running->pid;
    timers[running->pid].time = atoi(cmd);
    ksleep(&running->pid);
    
//     if (strcmp(cmd, "ps")==0)
//       do_ps();
//     if (strcmp(cmd, "fork")==0)
//       do_kfork();
//     if (strcmp(cmd, "switch")==0)
//       do_switch();
//     if (strcmp(cmd, "exit")==0)
//       do_exit();
//    if (strcmp(cmd, "sleep")==0)
//       do_sleep();
//    if (strcmp(cmd, "wakeup")==0)
//       do_wakeup();
//    if (strcmp(cmd, "wait")==0)
//       do_wait();
  }
}

int kfork()  // kfork a child process to execute body() function
{
  int i;
  PROC *tester;
  PROC *p = dequeue(&freeList);
  if (p==0){
    kprintf("kfork failed\n");
    return -1;
  }
  p->ppid = running->pid;       // set ppid
  p->parent = running;          // set parent PROC pointer
  p->status = READY;
  p->priority = 1;
  //p->child = NULL;
  //p->sibling = NULL;

  tester = running->child; // Pointer to check if child/siblings exist
  
  if (tester == NULL) { // If no child in parent
    running->child = p; // Child is now p
  } else {
    while (tester->sibling) { // While there are siblings
      tester = tester->sibling; // Move down sibling chain
    }
    tester->sibling = p; // The new sibling is p
  }

  for (i=1; i<15; i++)
    p->kstack[SSIZE-i] = 0;        // zero out kstack

  p->kstack[SSIZE-1] = (int)body;  // saved lr -> body()
  p->ksp = &(p->kstack[SSIZE-14]); // saved ksp -> -14 entry in kstack
 
  enqueue(&readyQueue, p);
  return p->pid;
}

int do_wait() {
  return kwait(0);
}

int do_kfork()
{
   int child = kfork();
   if (child < 0)
      printf("kfork failed\n");
   else{
      printf("proc %d kforked a child = %d\n", running->pid, child); 
      printList("readyQueue", readyQueue);
   }
   return child;
}

int do_switch()
{
   tswitch();
}

int do_exit()
{
  int exitCode;
  printf("enter an exitCode value : ");
  exitCode = geti();
  kexit(exitCode);
}

int do_sleep()
{
  int event;
  printf("enter an event value to sleep on : ");
  event = geti();
  ksleep(event);
}

int do_wakeup()
{
  int event;
  printf("enter an event value to wakeup with : ");
  event = geti();
  kwakeup(event);
}

int main()
{ 
   color = WHITE;
   row = col = 0; 

   fbuf_init();
   kprintf("Welcome to Wanix in ARM\n");
   kbd_init();

   VIC_INTENABLE |= (1<<4);  // timer0,1 at bit4 
   printf("test timer driver by interrupts\n");
   timer_init();        // initialize timer driver    
   timer_start(0);      // start timer 0
   
   /* enable VIC to route SIC interrupts */
   VIC_INTENABLE |= (1<<31); // SIC to VIC's IRQ31

   /* enable SIC to route KBD IRQ */
   SIC_ENSET |= (1<<3);      // KBD int=3 on SIC

   init();

   printQ(readyQueue);
   kfork();   // kfork P1 into readyQueue  

   unlock();
   while(1){
     if (readyQueue)
        tswitch();
   }
}

/*********** scheduler *************/
int scheduler()
{ 
  
  printf("proc %d in scheduler()\n", running->pid);
  if (running->status == READY)
     enqueue(&readyQueue, running);
  printList("readyQueue", readyQueue);
  running = dequeue(&readyQueue);
  printf("next running = %d\n", running->pid);  
  color = running->pid;
  
}


