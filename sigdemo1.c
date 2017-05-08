#include <stdio.h>
#include <signal.h>

void f(int); 

int main()
{
  int i;
  signal(SIGINT, f); 
  for(i=0; i<5; i++)
  {
    printf("hello\n");
    sleep(1); 
  }
}

void f(int sig_number)
{
  int input; 
  printf("Interrupted! OK to quit (y/n)?"); 
  while(input = getchar())
  {
    if(input == 'n' || input =='N')
      return; 
    if(input == 'y' || input == 'Y')
      exit(1); 
    if(input != 0x0a)
     printf("input is not 'y' or 'n', please input again\n");
  }
}
