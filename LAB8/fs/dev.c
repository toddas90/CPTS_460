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

int prchar(){ } // dummy printer function

/******************************
struct devsw {
  int (*dev_read)();
  int (*dev_write)();
} devsw[];
******************************/

struct devsw devsw[ ];
int nocall(){ }

int ubread(int device, int blk, char *ubuf)
{
  int i, dev;
  struct buf *bp;

  dev = device >> 8;         // major number only 2=FD, 3=HD
  bp = bread(dev, blk);  

  for (i=0; i<1024; i++){
      put_ubyte(bp->buf[i], ubuf++);
  }
  brelse(bp);
  return 1024;
}

int ubwrite(int device, int blk, char *ubuf)
{
  int i, dev;
  struct buf *bp;

  dev = device >> 8;                // dev =2 (FD or 3 (HD)
  bp = (struct buf *)getblk(dev, blk);

  for (i=0; i<1024; i++)
    bp->buf[i] = get_ubyte(ubuf++);   // copy block from U space to bp->buf[]

  bwrite(bp);

  return 1024;
}


int conread(), conwrite(), serread(), serwrite(), prwrite();

struct devsw devsw[] =
{ //   read         write  
  // --------     --------
      nocall,    nocall,          // /dev/null
      nocall,    nocall,          // 1 kcode
      ubread,    ubwrite,         // 2 FD
      ubread,    ubwrite,         // 3 HD
      conread,   conwrite,        // 4 Console
      serread,   serwrite,        // 5 Serial
      nocall,    prwrite          // 6 printer
};

int ugets(char *buf)
{
  return 0;
}
int conread(int dev, char *buf, int nbytes)
{
   int n, r;
   char *cp;

   // call ugets(), whcih puts chars into Ubuf directly */
   if (nbytes==1){ // getc() call from umode
      r = kgetc();
      //printf("conread: r= %c %x\n", r, r);
      put_ubyte(r, buf);
   
      if (r==4) 
         return 0; 
       return 1;
   }
   n = ugets(buf);
   return(n);
}

extern UART uart[4];

int serread(int dev, char *buf, int nbytes)
{
  int n, r;
  UART *tty;

  tty  = &uart[0];
  if ((dev & 0x000F) == 1){
     tty = &uart[1];
  }

  if (nbytes==1){
     r = sgetc(tty);
     put_ubyte(r, buf);
     if (r==4) 
        return 0;
     return 1;
  }
  n = usgets(tty, buf);
  return n;
}

int conwrite(int dev, char *buf, int nbytes)
{  
  char *cp; 
  int i;

  i = 0;
  cp = buf;
  
  while (i < nbytes){
    kputc(*cp);
    cp++, i++;
  }
  return nbytes;
}

int serwrite(int dev, char *buf, int nbytes)
{
  char *cp, c; 
  int i;
  UART *tty;

  tty  = &uart[0];
  if ((dev & 0x000F) == 1){
     tty = &uart[1];
  }

  cp = buf;
  i = 0;

  while (i < nbytes){
    c = *cp;
    sputc(tty, c);
    cp++; i++;
  }
  return nbytes;
}


int prwrite(int dev, char *buf, int nbytes)
{
  char *cp, c; 
  int i;

  cp = buf; i = 0;
  while (i < nbytes){
    c = get_ubyte(cp);
    prchar(c);
    cp++; i++;
  }

  return nbytes;
}


