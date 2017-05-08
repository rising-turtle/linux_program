#include "pp_table.h"

char table_row_c[RIGHT_EDGE - LEFT_EDGE + 1] = {'-'}; 

void draw_table()
{
  int row = TOP_ROW+1;
  int col = LEFT_EDGE; 
  int bot_row = row + g_net_height;
  for(; col<= RIGHT_EDGE; col++)
  {
    mvaddch(TOP_ROW, col, '-');
    // mvaddch(BOT_ROW, col, '-'); 
    mvaddch(bot_row, col, '-');
  }
  // for(;row<BOT_ROW; row++)
  for(; row<bot_row; row++)
  {
    if(g_client_or_server == CLIENT_TTY)
      mvaddch(row, LEFT_EDGE, '.');
    if(g_client_or_server == SERVER_TTY)
      mvaddch(row, RIGHT_EDGE, '.');
  }
}

void draw_paddle()
{
  int col = the_paddle.pad_col; 
  int row = the_paddle.pad_top; 
  for(; row<= the_paddle.pad_bot; ++row)
    mvaddch(row, col, the_paddle.pad_char);
}

void erase_paddle()
{
  int col = the_paddle.pad_col; 
  int row = the_paddle.pad_top; 
  for(; row<= the_paddle.pad_bot; ++row)
    mvaddch(row, col, BLANK);
}


