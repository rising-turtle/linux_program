#include "pp_table.h"

char table_row_c[RIGHT_EDGE - LEFT_EDGE + 1] = {'-'}; 

void draw_table()
{
  int row = TOP_ROW+1;
  int col = LEFT_EDGE; 
  for(; col<= RIGHT_EDGE; col++)
  {
    mvaddch(TOP_ROW, col, '-');
    mvaddch(BOT_ROW, col, '-'); 
  }
  for(;row<BOT_ROW; row++)
  {
    mvaddch(row, LEFT_EDGE, '|');
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


