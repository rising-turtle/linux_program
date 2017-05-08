#include "paddle.h"
#include "bounce.h"

struct pppaddle the_paddle; 

void paddle_init()
{
  int bot_row = TOP_ROW + g_net_height; 
  the_paddle.pad_top = TOP_ROW + (bot_row - TOP_ROW)/2; 
  the_paddle.pad_bot = the_paddle.pad_top + PADDLE_LEN - 1; 
  if(g_client_or_server == CLIENT_TTY)
    the_paddle.pad_col = RIGHT_EDGE; 
  else if(g_client_or_server == SERVER_TTY)
    the_paddle.pad_col = LEFT_EDGE;
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
  if( (x + 1 != the_paddle.pad_col && g_client_or_server == CLIENT_TTY) ||
      (x - 1 != the_paddle.pad_col && g_client_or_server == SERVER_TTY))
    return false; 
  return true;
}

