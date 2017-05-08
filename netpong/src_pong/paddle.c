#include "paddle.h"
#include "bounce.h"

struct pppaddle the_paddle; 

void paddle_init()
{
  the_paddle.pad_top = TOP_ROW + (BOT_ROW - TOP_ROW)/2; 
  the_paddle.pad_bot = the_paddle.pad_top + PADDLE_LEN - 1; 
  the_paddle.pad_col = RIGHT_EDGE; 
  the_paddle.pad_char = '#';
}

void paddle_up()
{
  if(the_paddle.pad_top != TOP_ROW + 1)
  {
    --the_paddle.pad_top; 
    --the_paddle.pad_bot;
  }
}

void paddle_down()
{
  if(the_paddle.pad_bot != BOT_ROW -1)
  {
    ++the_paddle.pad_top;
    ++the_paddle.pad_bot;
  }
}

bool paddle_contact(int y, int x)
{
  if(y < the_paddle.pad_top || y > the_paddle.pad_bot) 
    return false; 
  if(x + 1 != the_paddle.pad_col)
    return false; 
  return true;
}

