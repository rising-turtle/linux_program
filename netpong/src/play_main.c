#include "pp_table.h"
#include <stdlib.h>
#include <signal.h>

void set_up(); 
void serve(); 
void rand_speed();
void wrap_up();

struct ppball the_ball; 
int balls_left = 3;

int main()
{
  char c;
  srand(getpid()); 
  set_up();  /* init all stuff*/
  serve();   /* start up ball*/
  while( balls_left > 0 && (c = getchar()) != 'Q')
  {
    if(c == 'j')
    {
      erase_paddle();
      paddle_down(); 
    }
    else if(c == 'k')
    {
      erase_paddle();
      paddle_up();
    }
    draw_paddle(); 
    refresh(); 
  }
  wrap_up();
}

void set_up()
/*
 *	init structure and other stuff
 */
{
	void	ball_move(int);

	the_ball.y_pos = Y_INIT;
	the_ball.x_pos = X_INIT;
	the_ball.y_ttg = the_ball.y_ttm = Y_TTM ;
	the_ball.x_ttg = the_ball.x_ttm = X_TTM ;
	the_ball.y_dir = 1  ;
	the_ball.x_dir = 1  ;
	the_ball.symbol = DFL_SYMBOL ;

	initscr();
	noecho();
	crmode();

	signal( SIGINT , SIG_IGN );
	mvaddch( the_ball.y_pos, the_ball.x_pos, the_ball.symbol  );
       
        paddle_init(); 
        draw_table();
        draw_paddle();
        
	refresh();
	
	signal( SIGALRM, ball_move );
	set_ticker( 1000 / TICKS_PER_SEC );	/* send millisecs per tick */
}

void rand_speed_x()
{
  the_ball.x_ttg = the_ball.x_ttm = (rand())%X_MAX_TTM + 1;
}

void rand_speed_y()
{
  the_ball.y_ttg = the_ball.y_ttm = (rand())%Y_MAX_TTM + 1;
}

void rand_speed()
{
  rand_speed_x();
  rand_speed_y();
}

void bounce_speed_x()
{
  rand_speed_x(); 
  if(the_ball.y_ttm < the_ball.x_ttm)
  {
    the_ball.y_ttg = the_ball.y_ttm = the_ball.x_ttm;
  }
}

void bounce_speed_y()
{
  rand_speed_y(); 
  if(the_ball.x_ttm < the_ball.y_ttm)
  {
    the_ball.x_ttg = the_ball.x_ttm = the_ball.y_ttm;
  }
}

void serve()
{
  rand_speed();
  // the_ball.y_ttg = the_ball.y_ttm = (rand()+1)%Y_MAX_TTM;
  // the_ball.x_ttg = the_ball.x_ttm = (rand()+1)%X_MAX_TTM;
  the_ball.y_dir = (rand()%2)?1:-1; 
  the_ball.x_dir = (rand()%2)?1:-1;
}

void ball_move(int signum)
{
	int   y_cur, x_cur, moved;
        int   ball_status;  // NO_BOUNCE, BOUNCE, MISSING

	signal( SIGALRM , SIG_IGN );		/* dont get caught now 	*/
	y_cur = the_ball.y_pos ;		/* old spot		*/
	x_cur = the_ball.x_pos ;
	moved = 0 ;

	if ( the_ball.y_ttm > 0 && the_ball.y_ttg-- == 1 ){
		the_ball.y_pos += the_ball.y_dir ;	/* move	*/
		the_ball.y_ttg = the_ball.y_ttm  ;	/* reset*/
		moved = 1;
	}

	if ( the_ball.x_ttm > 0 && the_ball.x_ttg-- == 1 ){
		the_ball.x_pos += the_ball.x_dir ;	/* move	*/
		the_ball.x_ttg = the_ball.x_ttm  ;	/* reset*/
		moved = 1;
	}

	if ( moved ){
		mvaddch( y_cur, x_cur, BLANK );
		mvaddch( y_cur, x_cur, BLANK );
		mvaddch( the_ball.y_pos, the_ball.x_pos, the_ball.symbol );
		ball_status = bounce_or_lose( &the_ball );
                if(ball_status == MISSING) // lose 
                {
                  printw("MISSING BALL!"); 
                  mvaddch(the_ball.y_pos, the_ball.x_pos, BLANK);
                  if(--balls_left == 0) // no more games 
                  {
                    printw("GAME OVER!"); 
                    signal(SIGALRM, SIG_IGN);
                    refresh(); 
                    return ;
                  }else
                  {
                    erase_paddle();
                    set_up(); 
                    serve();
                  }
                }else if(ball_status == BOUNCE)
                {
                  // printw("BOUNCE BALL!"); 
                  // rand_speed(); 
                }else if(ball_status == NO_BOUNCE)
                {

                }
		move(LINES-1,COLS-1);
		refresh();
	}
	signal( SIGALRM, ball_move);		/* for unreliable systems */

}

int bounce_or_lose(struct ppball *bp)
{
	int	return_val = 0 ;

	if ( bp->y_pos == TOP_ROW+1 ){
		bp->y_dir = 1 ; 
		return_val = 1 ;
                // rand_speed_y();
                bounce_speed_y(); 
	} else if ( bp->y_pos == BOT_ROW-1 ){
		bp->y_dir = -1 ;
	       	return_val = 1;
                // rand_speed_y();
                bounce_speed_y(); 
	}

	if ( bp->x_pos == LEFT_EDGE+1 ){
		bp->x_dir = 1 ;
	       	return_val = 1 ;
                // rand_speed_x();
                bounce_speed_x();
	} else if ( bp->x_pos == RIGHT_EDGE-1 )
        {
              // check whether the ball contacts with the paddle
		bp->x_dir = -1;
	       	// return_val = 1;
                if(paddle_contact(bp->y_pos, bp->x_pos))
                  return_val = 1;
                else
                  return_val = MISSING;
                // rand_speed_x();
                bounce_speed_x();
	}

	return return_val;
}


void wrap_up()
{

	set_ticker( 0 );
	endwin();		/* put back to normal	*/
}




