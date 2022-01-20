/************** tt.c file **************/
// uprintf(char *fmt, ...) is ufprintf(&uart[0], char *fmt, ...)

int main()
{
  int i;
  char string[64]; 
  UART *up;
  
  N = 10;

  uart_init();

  up = &uart[0];
  uprintf("Enter lines from UART terminal, enter quit to exit\n");
  
  while(1){
    ugets(up, string);
    uprintf("    ");
    if (strcmp(string, "quit")==0)
       break;
    uprintf("%s\n", string);
  }
  uprintf("\n");

  uprintf("Compute sum of array\n");
  sum = 0;
  for (i=0; i<N; i++)
    sum += v[i];
  uprintf("sum = %d\n", sum);
  uprintf("END OF UART DEMO\n");
}
