#include "keymap2"

#define KCNTL 0x00
#define KSTAT 0x04
#define KDATA 0x08
#define KCLK  0x0C
#define KISTA 0x10

typedef volatile struct kbd{
  char *base;
  char buf[128];
  int head, tail, data, room;
}KBD;

volatile KBD kbd;

int release;       // key release flag
int shift;         // Shift release flag
int ctrl;          // Control release flag

int kbd_init()
{
  KBD *kp = &kbd;
  kp->base = (char *)0x10006000;
  *(kp->base + KCNTL) = 0x10;    // bit4=Enable bit0=INT on
  *(kp->base + KCLK)  = 8;       // ARM manual says clock=8
  kp->head = kp->tail = 0;       // circular buffer char buf[128]
  kp->data = 0; kp->room = 128;

  release = 0;
}

void kbd_handler()
{
  u8 scode, c;
  KBD *kp = &kbd;

  color = YELLOW;

  scode = *(kp->base + KDATA);   // get scan code of this interrpt

  kputs("kbd interrupt scancode = "); kprintx(scode);

  if (scode == 0xF0){  // it's key release 
     release = 1;      // set release flag
     return;
  }

  if (scode == 0x12) // Set shift flag
      shift = 1;

  if (scode == 0x14) // Set control flag
      ctrl = 1;

  if (release == 1){   // scan code after 0xF0
      if (scode == 0x12) // Detect when shift is released
          shift = 0;
      if (scode == 0x14) // Detect when ctrl is released
          ctrl = 0;
     release = 0;      // reset release flag
     return;
  }

  if (ctrl && scode == 0x21) { // If ctrl is set and scode is 'c'
      color = GREEN;
      kputs("Control-C key pressed!\n");
      color = YELLOW;
  } else if (ctrl && scode == 0x23) { // If ctrl is set and scode is 'd'
        color = GREEN;
        kputs("Control-D key pressed!\n");
        c = ltab[0x04];
        color = YELLOW;
  } else if (shift) { // If shift is set
      c = utab[scode];
  } else { // lowercase
      c = ltab[scode];
  }

  if (c) { // This if just makes the output easier to read.
    color = BLUE;
    kputs("kbd interrupt : "); kputc(c); kputs("\n");
    color = YELLOW;
  } else {
      kputs("kbd interrupt : "); kputc(c); kputs("\n");
  }

  kp->buf[kp->head++] = c;
  kp->head %= 128;
  kp->data++; kp->room--;
}

int kgetc()
{
  char c;
  KBD *kp = &kbd;

  unlock();                          // unmask IRQ in case it was masked out
  while(kp->data == 0);              // BUSY wait while kp->data is 0 

  lock();                            // mask out IRQ
    c = kp->buf[kp->tail++]; 
    kp->tail %= 128;                 /*** Critical Region ***/
    kp->data--; kp->room++;
    unlock();                        // unmask IRQ
  return c;
}

int kgets(char s[ ])
{
  char c;
  while( (c = kgetc()) != '\r'){
    *s = c;
    s++;
  }
  *s = 0;
}

