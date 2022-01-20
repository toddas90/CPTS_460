char *tab = "0123456789ABCDEF";
#define DR 0x00
#define FR 0x18

typedef struct uart{
  char *base;           // base address
  int  n;
}UART;

UART uart[4];          // 4 UART structs

// For versatile_epb : uarts are at 0x101F1000, 2000, 3000; 10009000 

int uart_init()
{
  int i; UART *up;
  for (i=0; i<4; i++){
    up = &uart[i];
    up->base = (char *)(0x10009000 + i*0x1000);
   // up->base = (char *)(0x101f1000 + i*0x1000);
    up->n = i;
  }
  uart[3].base = (char *)(0x10009000);
}

int ugetc(UART *up)
{
  while ( *(up->base + FR) & 0x10 );
  return (char)(*(up->base + DR));
}

int uputc(UART *up, char c)
{
  while ( *(up->base + FR) & 0x20 );
  *(up->base + DR) = (int)c;
}

int ugets(UART *up, char *s)
{
  while ((*s = (char)ugetc(up)) != '\r'){
    uputc(up, *s);
    s++;
  }
 *s = 0;
}

int uprints(UART *up, char *s)
{
  while(*s)
    uputc(up, *s++);
}
