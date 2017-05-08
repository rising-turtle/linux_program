#ifndef PADDLE_H
#define PADDLE_H

#define PADDLE_LEN 6
#include <stdbool.h>

struct pppaddle{
  int  pad_top, pad_bot, pad_col; 
  char pad_char;
};

extern struct pppaddle the_paddle; 

extern void paddle_init(); 
extern void paddle_up(); 
extern void paddle_down(); 
extern bool paddle_contact(int y, int x);

#endif
