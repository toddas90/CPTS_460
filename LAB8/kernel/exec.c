/********************************************************************
Copyright 2010-2017 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/

#include "../type.h"

int body(), goUmode();

int exec(char *uline)
{
  int i, *ip;
  char *cp, kline[64]; 
  PROC *p = running;
  char file[32], filename[32];
  int *usp, *ustacktop;
  u32 BA, Btop, Busp, uLINE;
  char line[32];

  //printf("EXEC: proc %d uline=%x ", running->pid, uline);

  // line is in Umode image at p->pgdir[2048]&0xFFF00000=>can access from Kmode
  // char *uimage = (char *)(p->pgdir[2048] & 0xFFF00000);
  BA = (p->res->pgdir[2048] & 0xFFFFF000);
  Btop = BA + 0x100000;  // top of 1MB Uimage
  //printf("EXEC: proc %d Uimage at %x\n", running->pid, BA);
 
  uLINE = BA + ((int)uline - 0x80000000);
  kstrcpy(kline, (char *)uLINE);
  // NOTE: may use uline directly 

  //printf("EXEC: proc %d line = %s   ", running->pid, kline); 

  // first token of kline = filename
  cp = kline; i=0;
  while(*cp != ' ' && *cp){
    filename[i] = *cp;
    i++; cp++;
  } 
  filename[i] = 0;
  /*
  kstrcpy(file, "/bin/");
  kstrcat(file, filename);
  */
  BA = p->res->pgdir[2048] & 0xFFFFF000;
  //kprintf("load file %s to %x\n", file, BA);

  // load filename to Umode image 
  if (load(filename, p) <= 0 ){
    printf("exec loading error\n");
    return -1;
  }
  //  printf("after loading ");

  // copy kline to high end of Ustack in Umode image
  Btop = BA + 0x100000;
  Busp = Btop - 32;

  cp = (char *)Busp;
  kstrcpy(cp, kline);
  //printf("cp=%x contents=%s\n", cp, cp);

  p->usp = (int *)VA(0x100000 - 32);
  p->upc = (int *)VA();
  
  p->kstack[SSIZE-14] = (int)VA(0x100000 - 32); // R0 to Umode
  p->kstack[SSIZE-1] = (int)VA(0);              // ulr to Umode
  //printf("usp=%x contents=%s\n", p->usp, (char *)p->usp);
  strcpy(running->res->name, filename);

  //kprintf("kexec exit\n");
  return p->usp;       // return value may ovewrite saved R0 in kstack
}

int kfe(char *cmdline) {
    kprintf("Beginning Fork process\n");
    int i = 0;
    int *ptable = NULL;
    int pentry = 0;
    char *PA = NULL;
    char *CA = NULL;
    PROC *p = getproc(&freelist);

    if (p == 0) {
        kprintf("Fork: no more PROCs\n");
        return -1;
    }
    kprintf("New PROC %d\n", p->pid);

    p->status = READY;
    p->ppid = running->pid;
    p->parent = running;
    p->priority = 1;
    p->inkmode = 1;
    p->time = 0;
    p->cpu = 0;
    p->type = PROCESS;
    p->cpsr = 0x10;
    
    p->res->size = running->res->size;
    p->res->uid = running->res->uid;
    p->res->gid = running->res->gid;
    p->res->cwd = running->res->cwd;
    p->tcount = 1;
    p->res->cwd->refCount++;
    strcpy(p->res->tty, running->res->tty);
    
    // p->res->signal, p->res->sig[] are cleared in kexit()
    p->res->signal = 0;
    for (i=0; i<NSIG; i++)
        p->res->sig[i] = 0;
    /***** clear message queue ******/
    p->res->mqueue = 0; 
    p->res->mlock.value = 1; p->res->mlock.queue = 0;
    p->res->message.value = 0; p->res->message.queue = 0;
    
    // p->status = READY;
    // p->priority = 1;
    // p->ppid = running->pid;
    // p->parent = running;
    kprintf("PROC values set\n");

    p->res->pgdir = (int *)(0x600000 + (p->pid-1)*0x4000);

    ptable = p->res->pgdir;
    for (i = 0; i < 4096; i++) {
        ptable[i] = 0;
    }

    pentry = 0x412;
    for (i = 0; i < 258; i++) {
        ptable[i] = pentry;
        pentry += 0x100000;
    }

    ptable[2048] = (0x800000 + (p->pid-1)*0x100000) | 0xC32;
    kprintf("Page table set\n");

    PA = (char *)(running->res->pgdir[2048] & 0xFFFF0000);
    CA = (char *)(p->res->pgdir[2048] & 0xFFFF0000);
    kprintf("PA=%x CA=%x\n", PA, CA);

    //memcpy(CA, PA, 0x100000);
    //kprintf("memcpy done\n");

    for (i = 1; i < 14; i++) {
        p->kstack[SSIZE - i] = running->kstack[SSIZE - i];
    }

    p->kstack[SSIZE - 14] = 0;
    p->kstack[SSIZE - 15] = (int)goUmode;
    p->ksp = &(p->kstack[SSIZE - 28]);
    p->usp = (int *)running->usp;
    p->cpsr = (int *)running->cpsr;
    kprintf("Proc is ready!\n");

    // enqueue(&readyQueue, p);
    // return p->pid;

    kprintf("Time to exec!\n");

    i = 0;
    int upa = 0;
    int usp = 0;
    char *cp = NULL;
    char kline[128];
    char file[32];
    char filename[32];

    strcpy(kline, cmdline);
    cp = kline; i = 0;
    while (*cp != ' ') {
        filename[i] = *cp;
        i++; cp++;
    }

    filename[i] = 0;
    file[0] = 0;

    kstrcat(file, filename);
    kprintf("file found=%s\n", file);

    upa = p->res->pgdir[2048] & 0xFFFF0000;

    if (load(file, p) <= 0) {
        printf("exec loading error\n");
        return -1;
    }
    kprintf("Loaded file %s to %x\n", file, upa);

    usp = upa + 0x100000 - 128;
    kprintf("usp=%x\n", usp);

    strcpy((char *)usp, kline);
    p->usp = (int *)VA(0x100000 - 128);
    kprintf("p->usp=%x\n", p->usp);

    for (i = 0; i < 14; i++) {
        p->kstack[SSIZE - i] = 0;
    }
    kprintf("kstack set\n");

    p->kstack[SSIZE - 1] = (int)VA(0);

    kprintf("Returning from kfe\n");
    
    // return p->usp;
    return p->pid;
}
