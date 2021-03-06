/**************** t.c file **************/
int N;
int v[] = {1,2,3,4,5,6,7,8,9,10};
int sum;

#include "string.c"
#include "uart.c"

int main()
{
  int i;
  char string[64];
  UART *up;
  
  N = 10;

  uart_init();
  up = &uart[0]; // default was 0

  uprintf(up, "Enter lines from UART terminal, enter \"quit\" to exit\n\r");
  
  while(1){
    ugets(up, string);
    uprintf(up, "Recieved: %s\n\r", string);
    
    if (strcmp(string, "quit")==0)
       break;
  }
  uprintf(up, "Compute sum of array\n\r");
  sum = 0;
  for (i=0; i<N; i++)
    sum += v[i];
  uprintf(up, "sum = %d\n\r", sum);
 
  uprintf(up, "END OF UART DEMO\n\r");
}
