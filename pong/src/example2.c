#define MAXROW 1000
#define MAXCOL 500 

#include <curses.h> 
#include <string.h>
#include <stdio.h>
WINDOW * scrn; 

char cmdoutlines[MAXROW][MAXCOL]; 

int ncmdlines, 
    nwinlines, 
    winrow, 
    cmdstartrow, 
    cmdlastrow; 

// rewrite the lines at winrow in bold model 
void highlight()
{
  int clinenum; 
  attron(A_BOLD); // BOLD font 
  clinenum = cmdstartrow + winrow; 
  mvaddstr(winrow, 0, cmdoutlines[clinenum]); 
  attroff(A_BOLD); // cancle BOLD model 
  refresh(); 
}

// run "ps ax" and stores the output in cmdoutlines  
void runpsax()
{
  FILE *p; char ln[MAXCOL]; int row; char* tmp;
  p = popen("ps ax","r"); // open Unix pipe (enables one program to read
  // output of another as if it were a file)
  for (row = 0; row < MAXROW; row++) {
    tmp = fgets(ln,MAXCOL,p); // read one line from the pipe
    if (tmp == NULL) break; // if end of pipe, break
    // don’t want stored line to exceed width of screen, which the
    // curses library provides to us in the variable COLS, so truncate
    // to at most COLS characters
    strncpy(cmdoutlines[row],ln,COLS);
    // remove EOL character
    cmdoutlines[row][MAXCOL-1] = 0;
  }
  ncmdlines = row;
  close(p); // close pipe
}


// displays last part of command output (as much as fits in screen)
void showlastpart()
{ 
  int row;
  clear(); // curses clear-screen call
  // prepare to paint the (last part of the) ’ps ax’ output on the screen
  // two cases, depending on whether there is more output than screen rows;
  // first, the case in which the entire output fits in one screen:
  if(ncmdlines <= LINES )
  {
    cmdstartrow = 0;
    nwinlines = ncmdlines;

  }
  else { // now the case in which the output is bigger than one screen
    cmdstartrow = ncmdlines - LINES;
    nwinlines = LINES;
  }
  cmdlastrow = cmdstartrow + nwinlines - 1;
  // now paint the rows to the screen
  for (row = cmdstartrow, winrow = 0; row <= cmdlastrow; row++,winrow++)
    mvaddstr(winrow,0,cmdoutlines[row]); // curses call to move to the
  // specified position and
  // paint a string there
  refresh(); // now make the changes actually appear on the screen,
  // using this call to the curses library
  // highlight the last line
  winrow--;
  highlight();
}

// moves up/down one line
void updown(int inc)
{ int tmp = winrow + inc;
  // ignore attempts to go off the edge of the screen
  if (tmp >= 0 && tmp < LINES) {
    // rewrite the current line before moving; since our current font
    // is non-BOLD (actually A_NORMAL), the effect is to unhighlight
    // this line
    mvaddstr(winrow,0,cmdoutlines[cmdstartrow+winrow]);
    // highlight the line we’re moving to
    winrow = tmp;
    highlight();
  }
}

// run/re-run "ps ax"
void rerun()
{ runpsax();
  showlastpart();
}

// kills the highlighted process
void prockill()
{ 
  char *pid;
  // strtok() is from C library; see man page
  pid = strtok(cmdoutlines[cmdstartrow+winrow]," ");
  kill(atoi(pid),9); // this is a Unix system call to send signal 9,
  // the kill signal, to the given process
  rerun();
}

void main()
{ char c;
  // window setup, next 3 lines are curses library calls, a standard
  // initializing sequence for curses programs
  scrn = initscr();
  noecho(); // don’t echo keystrokes
  cbreak(); // keyboard input valid immediately, not after hit Enter
  // run ’ps ax’ and process the output
  runpsax();
  // display in the window
  showlastpart();
  // user command loop
  while (1) {
    // get user command
    c = getch();
    if (c == 'u') updown(-1);
    else if (c == 'd') updown(1);
    else if (c == 'r') rerun();
    else if (c == 'k') prockill();
    else break; // quit

  }
  endwin();
}
