#include <curses.h>

int r,c, 
    nrows, 
    ncols; 
void draw(char dc);

int main()
{
  int i; char d; 
  WINDOW * wnd; 
  wnd = initscr();  // curses call to initialize window
  cbreak();         // curses call to set no waiting for Enter key 
  noecho();         // curses call to set no echoing 
  getmaxyx(wnd, nrows, ncols);  // curses all to find size of window 
  clear();          // curses call to clear screen, and set current position to (0,0)
  refresh();        // curses call to implement all changes since last fresh 

  r = 0; c= 0; 
  while(1)
  {
    d = getch(); // curses call to get an input from keyboards
    if(d == 'q' || d== 'Q') break; 
    draw(d); 
  }
  endwin();         // curses call to restore the original window and leave 
}


void draw(char dc)
{
  move(r,c); 
  delch(); 
  insch(dc);
  refresh(); 
  r++;
  if(r == nrows)
  {
    r = 0; 
    c++; 
    if(c==ncols) c = 0;
  }
}
