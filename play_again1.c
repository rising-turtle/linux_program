/*
 * play_again1.c
 *  purpose: ask if user wants another transaction 
 *   method: set tty into char-by-char mode, read char, return result 
 *  returns: 0=>yes, 1=>no
 *   better: do no echo inappropriate input
 * */

#include <stdio.h>
#include <termios.h>

#define QUESTION "Do you want another transaction?"

void tty_mode(int mode); 
void set_crmode(); 
int get_response(const char* q); 

int main(int argc, char* argv[])
{
  int response; 
  tty_mode(0);    // save tty mode 
  set_crmode();   // set chr-by-chr mode
  response = get_response(QUESTION); // get some answer 
  tty_mode(1);    // restore tty mode 
  return 0; 
}

/*  
 *  purpose: ask a question and wait for a y/n answer 
 *  method: use getchar and complaine about non y/n answers 
 *  returns: 0-> yes, 1->no
 * */
int get_response(const char* q)
{
  int input; 
  printf("%s (y/n)\n", q); 
  while(1)
  {
    switch(input = getchar())
    {
      case 'y':
      case 'Y': return 0; 
      case EOF:
      case 'n':
      case 'N': return 1;
      default:
              printf("\ncannot understand %c, please type 'y' or 'n'\n", input); 
              break;
    }
  }
}

void set_crmode()
{
  struct termios ttystate; 
  tcgetattr(0, &ttystate); 
  ttystate.c_lflag &= ~ICANON;   // no buffering 
  ttystate.c_cc[VMIN] = 1;      // one character one time 
  tcsetattr(0, TCSANOW, &ttystate); // install settings
}

void tty_mode(int model)
{
  static struct termios original_mode; 
  if(model == 0)
    tcgetattr(0, &original_mode); 
  else 
    return tcsetattr(0, TCSANOW, &original_mode);
}


