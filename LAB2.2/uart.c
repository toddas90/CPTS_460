#include <stdarg.h>

typedef unsigned int u32;

char *tab = "0123456789ABCDEF";
#define DR 0x00
#define FR 0x18
#define RXFE 0x10
#define TXFF 0x20

typedef struct uart{
  char *base;           // base address
  int  n;
}UART;

UART uart[4];          // 4 UART structs
UART *up;

// For versatile_epb : uarts are at 0x101F1000, 2000, 3000; 10009000 
int uputc(UART *up, char c);

int uart_init()
{
  int i; //UART *up;
  for (i=0; i<4; i++){
    up = &uart[i];
    up->base = (char *)(0x10009000 + i*0x1000);
   // up->base = (char *)(0x101f1000 + i*0x1000);
    up->n = i;
  }
  uart[3].base = (char *)(0x10009000);
}

void uprintf(char *fmt, ...) {
    va_list valist;
    char *it = fmt;
    char *strval;

    va_start(valist, fmt);

    while(*it) {
        if(*it != '%') {
            uputc(up, *it);
            it++;
            continue;
        }
        switch(*++it) {
            case 'c':
                uputc(up, va_arg(valist, int));
                break;
            case 'd':
                uputc(up, va_arg(valist, int));
                break;
            case 'u':
                uputc(up, va_arg(valist, u32));
                break;
            case 'x':
                uputc(up, va_arg(valist, u32));
            case 'o':
                uputc(up, va_arg(valist, u32));
            case 's':
                strval = va_arg(valist, char *);
                while(*strval) {
                    uputc(up, *strval);
                    strval++;
                }
                break;
        }
        it++;
    }
    va_end(valist);
}

void uprintu(UART *up, u32 x) {
    int b = 10;
    (x==0) ? uputc(up, '0') : urpu(up, x, b);
}

void uprintd(UART *up, int x) {
    int b = 10;
    if(x < 0) {
        x = x * -1;
        uputc(up, '-');
    }
    (x == 0) ? uputc(up, '0') : urpu(up, x, b);
}

void uprintx(UART *up, u32 x) {
    int b = 16;
    (x == 0) ? uputc(up, '0') : urpu(up, x, b);
}

void uprinto(UART *up, u32 x) {
    int b = 8;
    (x == 0) ? uputc(up, '0') : urpu(up, x, b);
}

int urpu(UART *up, u32 x, int base) {
    char c;
    if (x) {
        c = tab[x % base];
        urpu(up, x / base, base);
        uputc(up, c);
    }
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
 uprints(up, "\n\r");
}

int uprints(UART *up, char *s)
{
  while(*s)
    uputc(up, *s++);
}
