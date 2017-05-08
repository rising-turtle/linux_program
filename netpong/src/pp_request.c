#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>
#include "pp_table.h"
#include "conn_reply.h"

void set_up(); 
void serve(); 
void rand_speed();
void wrap_up();
void ball_move(int);

void switch_to_serve(); 
void switch_to_play(); // 
void switch_to_wait(); 
void draw_ball_with_paddle();

void wait_for_reply(); // wait for opponent to reply 
void parse_reply(char* msg);  // parse the recv msg 

void print_bot_middle(char* msg); // for debug 
void print_bot_right(char* msg); 
void print_bot_left(char* msg); 
void print_top2_left(char* msg); 
void print_top1_left(char* msg); 
void print_score_result(); 
void print_play_status(); 

struct ppball the_ball; 
int balls_left = 3;

int request_pp_server(int fd, int port) // pp called by server 
{
  if((g_fid = fdopen(fd, "r")) == NULL)
  {
    return -1; 
  }
  g_fd = fd; 

  g_client_or_server = SERVER_TTY;
  
  // 1, server send HELO msg to client 
  char* name = "David"; 
  memcpy(g_player_name, name, strlen(name) + 1); 

  // initialize some parameters
  balls_left = 3; 
  g_my_score = 0; 
  g_pair_score = 0; 
  g_begin_play = 0;
  g_play_stat = WAIT; 
  
  tellHELO(g_pp_version, g_ticks_per_sec, g_net_height, name);
  
  // 2, wait for NAME reply 
  wait_for_reply(); 

  // 3, send SERV 
  // tellSERV(balls_left); 
  
  // 4, wait to play 
  // while(1)  // play the ball 
  {
    pp_start(); 
  }
  // 5, 
  printf("game over, %s\n", g_result_info); 
  fflush(stdout);
}

int request_pp_client(int fd, int port) // pp called by client
{
  if((g_fid = fdopen(fd, "r")) == NULL)
  {
    return -1; 
  }
  g_fd = fd;
  // printf("request_pp_client: g_fd = %d\n", g_fd);
  g_client_or_server = CLIENT_TTY; 

  char* name = "Hannah"; 
  memcpy(g_player_name, name, strlen(name) + 1); 

  // 1. wait for HELO, reply NAME 
  wait_for_reply(); 
  
  // 2. wait for SERV 
  wait_for_reply(); 

  // 3. start to play 
  pp_start(); 
 
  printf("game over, %s\n", g_result_info); 
}

void wait_for_reply() 
{
  // select different input 
  int stdin_id = 0; 
  int max_id = (g_fd > stdin_id ? g_fd: stdin_id) + 1;
  fd_set readfds; 
  struct timeval timeout;

  g_stop_wait = 0; // flag tells whether to wait more time 
  while(!g_stop_wait)
  {
    FD_ZERO(&readfds);   // clear flags everytime 
    FD_SET(stdin_id, &readfds); 
    FD_SET(g_fd, &readfds); 
    
    timeout.tv_sec = 10; // wait time 
    timeout.tv_usec = 0; 

    int retval = select(max_id, &readfds, NULL, NULL, &timeout); 
    if(retval == -1)
    {
      // printf("select return -1\n"); 
      // return ;
      continue; 
    }else if(retval > 0)
    {
      if(FD_ISSET(g_fd, &readfds)) // input from socket 
      {
        char buf[1024] = {0}; 
        // printf("pp_request.c: in wait_for_reply() \n");
        if(SCgetstr(g_fid, buf, 1024) == -1)
        {
          printf("pp_request.c: failed to getStr()\n"); 
          return ;
        }
        
        // analyze the msg from the other player  
        parse_reply(buf); // 

        // printf("pp_request.c: recv reply %s\n", buf); 
        // break; 
        if(g_stop_wait) 
        {
        //  printf("pp_request.c g_stop_wait = 1, break\n");
        }
      }
      if(FD_ISSET(stdin_id, &readfds)) // input from keyboard 
      {
        char c = getchar(); 
        if(g_begin_play)
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
        }else
        {
          printf("%c", c); 
          fflush(stdout);
        }
      }
    }
  }
  return ;
}

void parse_reply(char* msg)
{
  char tag[4] = {0}; 
  memcpy(tag, msg, 4); 
  char* cont = msg + 5; 
  if(g_begin_play)
  {
    // print_bot_middle(&tag[0]);
  }
  if(strncmp(tag, "HELO", 4) == 0) // HELO version tick_per_sec net_height name 
  {
    float v; int t_p_s, net_height; char name[32];
    sscanf(cont, "%f %d %d %s", &v, &t_p_s, &net_height, name); 
    printf("recv ----->HELO from %s version: %f tick_per_seconds: %d net_height: %d\n", name, v, t_p_s, net_height);
    // recv HELO from server 
    g_ticks_per_sec = t_p_s; 
    g_net_height = net_height; 
    memcpy(g_pair_name, name, strlen(name)+1);

    // reply NAME 
    tellNAME(g_pp_version, g_player_name); 

    g_stop_wait = 1; // wait for the next cmd
  }else if(strncmp(tag, "NAME", 4) == 0) // NAME name
  {
    char name[32]; 
    sscanf(cont, "%s", name); 
    printf("recv ------>NAME %s\n", name); 
    // recv NAME from client 
    memcpy(g_pair_name, name, strlen(name)+1); 
    
    // reply SERV 
    tellSERV(balls_left); 
    g_begin_play = 1;

    g_stop_wait = 1; // wait for the next cmd 

  }else if(strncmp(tag, "SERV", 4) == 0) // SERV number_of_pps
  {
    int n_pps; 
    sscanf(cont, "%d", &n_pps); 
    printf("recv ----->SERV %d, start game g_play_stat = %d! \n", n_pps, PLAY); 
    
    balls_left = n_pps;
    // g_play_stat = PLAY;  // client start to play the game 
    g_play_stat = SERV; 
    g_begin_play = 1;
    g_stop_wait = 1; // wait for the next cmd 
  }else if(strncmp(tag, "BALL", 4) == 0) // BALL y_pos x_ttm y_ttm y_dir 
  {
    int y_p, x_t, y_t, y_d; 
    sscanf(cont, "%d %d %d %d", &y_p, &x_t, &y_t, &y_d);
   
    the_ball.y_pos = y_p; 
    the_ball.y_ttg = the_ball.y_ttm = y_t; 
    the_ball.x_ttg = the_ball.x_ttm = x_t; 
    the_ball.y_dir = y_d; 
    if(g_client_or_server == CLIENT_TTY) // client end
    {
      the_ball.x_pos = LEFT_EDGE + 1; 
      the_ball.x_dir = 1; 
    }else if(g_client_or_server == SERVER_TTY) // server end 
    {
      the_ball.x_pos = RIGHT_EDGE -1; 
      the_ball.x_dir = -1;
    }
    switch_to_play(); 
    print_bot_middle("BALL MSG ");
  }else if(strncmp(tag, "MISS", 4) == 0) // MISS
  {
    ++g_my_score;
    --balls_left; 
    print_score_result();
    if(balls_left == 0) // DONE 
    {
      if(g_my_score > g_pair_score) 
      {
        tellDONE("Nice game, I win");
        sprintf(g_result_info, "I win, %s vs %s: %d:%d", g_player_name, g_pair_name, g_my_score, g_pair_score); 
      }
      else if(g_my_score < g_pair_score)
      {
        tellDONE("Nice game, you win");
        sprintf(g_result_info, "I lose, %s vs %s: %d:%d", g_player_name, g_pair_name, g_my_score, g_pair_score); 
      }
      g_stop_wait = 1;
      
    }else
    {
      switch_to_wait(); 
      set_up(); 
      // switch_to_play(); 
      switch_to_serve(); 
    }
  }else if(strncmp(tag, "DONE", 4) == 0) // DONE word
  {
    char word[128]; 
    sscanf(cont, "%s", word); 
    
    // tellQUIT("Let's play again soon"); 
    if(g_my_score > g_pair_score) 
    {
      sprintf(g_result_info, "I win, %s vs %s: %d:%d", g_player_name, g_pair_name, g_my_score, g_pair_score); 
    }
    else if(g_my_score < g_pair_score)
    {
      sprintf(g_result_info, "I lose, %s vs %s: %d:%d", g_player_name, g_pair_name, g_my_score, g_pair_score); 
    }
    g_stop_wait = 1; 

  }else if(strncmp(tag, "QUIT", 4) == 0) // QUIT word
  {
    char word[128]; 
    sscanf(cont, "%s", word);

    sprintf(g_result_info, "%s quit the game", g_pair_name); 
    g_stop_wait = 1; 
    
    switch_to_wait(); 

    if(g_client_or_server == CLIENT_TTY) // switch to server_tty
    {
      g_switch_to_server = 1;  
    }  
  }else
  {
    print_bot_middle("NOT RECOGNIZE?");
    // print_bot_right(&tag[0]);
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////  
// 
// pp ball play
//
// /////////////////
int pp_start()  // play pp 
{
  char c;
  srand(getpid()); 
  // printf("pp_request.c in pp_start: g_fd = %d g_play_stat = %d\n", g_fd, g_play_stat);
  set_up();  /* init all stuff*/
  if(g_play_stat == PLAY) // if I can play
  {
    switch_to_play(); 
  }else if(g_play_stat == SERV)
  {
    switch_to_serve(); 
  }

  // serve();   /* start up ball*/  

  // wait_for_reply();
  
  int stdin_id = 0; 
  int max_id = (g_fd > stdin_id ? g_fd: stdin_id) + 1;
  fd_set readfds; 
  struct timeval timeout;

  g_stop_wait = 0; // flag tells whether to wait more time 
  while(!g_stop_wait)
  {
    FD_ZERO(&readfds);   // clear flags everytime 
    FD_SET(stdin_id, &readfds); 
    FD_SET(g_fd, &readfds); 
    
    timeout.tv_sec = 10; // wait time 
    timeout.tv_usec = 0; 

    int retval = select(max_id, &readfds, NULL, NULL, &timeout); 
    if(retval == -1)
    {
      // printf("select return -1\n"); 
      // return ;
      continue; 
    }else if(retval > 0)
    {
      if(FD_ISSET(g_fd, &readfds)) // input from socket 
      {
        char buf[1024] = {0}; 
        // print_bot_middle("RECV MSG");
        if(SCgetstr(g_fid, buf, 1024) == -1)
        {
          printw("pp_request.c: failed to getStr()\n"); 
          return ;
        }

        // analyze the msg from the other player  
        parse_reply(buf); //     
      }
      if(FD_ISSET(stdin_id, &readfds)) // input from keyboard 
      {
        char c = getchar(); 
        if(g_begin_play)
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
          }else if(c == 's' || c== 'S')
          {
            // serve the ball 
            if(g_play_stat == SERV)
            {
              if(g_client_or_server == CLIENT_TTY)
                the_ball.x_dir = -1; 
              else if(g_client_or_server == SERVER_TTY)
                the_ball.x_dir = +1;
              the_ball.y_dir = rand()%2? 1:-1; 
              switch_to_play(); 
            }
          }else if(c == 'Q')
          {
            // send quit info 
            tellQUIT("I quit"); 
            g_stop_wait = 1; 
            switch_to_wait(); 
            if(g_client_or_server == SERVER_TTY)
            {
              g_server_quit = 1; 
            }
           continue;
          }
          draw_paddle(); 
          if(g_play_stat == SERV)
            draw_ball_with_paddle(); 
          move(LINES-1, COLS-1);
          refresh(); 
        }else
        {
          printf("%c", c); 
          fflush(stdout);
        }
      }
    
    }
  }

  wrap_up();
  // printf("pp_start: exit while loop\n");
}

void set_up()
/*
 *	init structure and other stuff
 */
{

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
	// mvaddch( the_ball.y_pos, the_ball.x_pos, the_ball.symbol  );
       
        paddle_init(); 
        draw_table();
        draw_paddle();
      
        // 
        char title[256] = {0}; 
        sprintf(title, "%s's view ", g_player_name); 
        print_top2_left(title); 
        print_score_result(); 
        print_play_status(); 

	refresh();
	
	// signal( SIGALRM, ball_move );
        // set_ticker(1000 / g_ticks_per_sec); 
}

void switch_to_play()
{
  g_play_stat = PLAY; // recv BALL msg, switch to play 
  mvaddch( the_ball.y_pos, the_ball.x_pos, the_ball.symbol  );
  move(LINES -1, COLS-1);
  refresh();
  signal( SIGALRM, ball_move );
  // set_ticker( 1000 / TICKS_PER_SEC );	/* send millisecs per tick */
  set_ticker(1000 / g_ticks_per_sec); 
  print_play_status("PLAY");
}

void switch_to_wait()
{
  g_play_stat = WAIT; // 
  mvaddch(the_ball.y_pos, the_ball.x_pos, BLANK); // clear the ball 
  refresh();  
  signal(SIGALRM, SIG_IGN);
  print_play_status("WAIT");
}

void draw_ball_with_paddle()
{
  mvaddch(the_ball.y_pos, the_ball.x_pos, BLANK); // clear the ball 
  the_ball.y_pos = the_paddle.pad_top + (the_paddle.pad_bot - the_paddle.pad_top)/2;
  if(g_client_or_server == CLIENT_TTY)
  {
    the_ball.x_pos = the_paddle.pad_col - 1; 
  }else
  {
    the_ball.x_pos = the_paddle.pad_col + 1; 
  }
  mvaddch(the_ball.y_pos, the_ball.x_pos, the_ball.symbol); // clear the ball 
}

void switch_to_serve()
{
  g_play_stat = SERV; 
  draw_ball_with_paddle(); 
  move(LINES-1, COLS-1); 
  refresh(); 
  signal(SIGALRM, SIG_IGN);
  print_play_status("SERV"); 
}

void print_bot_middle(char* msg) // for debug 
{
  move(BOT_ROW+1, LEFT_EDGE + (RIGHT_EDGE-LEFT_EDGE)/2);
  printw("%s", msg);
}

void print_bot_right(char* msg)
{
  move(BOT_ROW+1, RIGHT_EDGE); 
  printw("%s", msg);
}

void print_bot_left(char* msg)
{
  move(BOT_ROW+1, LEFT_EDGE); 
  printw("%s", msg); 
}

void print_top2_left(char* msg)
{
  move(TOP_ROW-2, LEFT_EDGE+7); 
  printw("%s", msg); 
}
void print_top1_left(char* msg)
{
  move(TOP_ROW-1, LEFT_EDGE+1); 
  printw("%s", msg);
}

void print_score_result()
{
  char res[256] = {0}; 
  sprintf(res, "%s : %d %s : %d LEFT: %d", g_player_name, g_my_score, g_pair_name, g_pair_score, balls_left); 
  print_top1_left(res);
}

void print_play_status()
{
  if(g_play_stat == WAIT)
  {
    print_bot_left("WAIT"); 
  }else if(g_play_stat == PLAY)
  {
    print_bot_left("PLAY");
  }
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
                  print_bot_middle("MISSING BALL!   ");
                  // printw("MISSING BALL!"); 
                  // mvaddch(the_ball.y_pos, the_ball.x_pos, BLANK); // clear the ball 
                  ++g_pair_score; 
                  --balls_left;
                  print_score_result();
                  switch_to_wait(); 
                  tellMISS(); 

                  if(balls_left == 0) // no more games 
                  {
                    // printw("GAME OVER!"); 
                    // signal(SIGALRM, SIG_IGN);
                    // tellDONE("finished");
                    // refresh(); 
                    // return ;
                  }else
                  {
                    // erase_paddle();
                    // set_up(); 
                    // serve();
                    
                    // send MISSING 
                    // tellMISS(); 
                    // g_play_stat = WAIT; 
                  }
                  return ;
                }else if(ball_status == BOUNCE)
                {
                  print_bot_middle("BOUNCE        "); 
                  // printw("BOUNCE BALL!"); 
                  // rand_speed(); 
                }else if(ball_status == NO_BOUNCE) 
                {

                }else if(ball_status == SWITCH)
                {
                  print_bot_middle("SWITCH to WAIT");
                  switch_to_wait(); 
                  tellBALL(the_ball.y_pos, the_ball.x_ttm, the_ball.y_ttm, the_ball.y_dir);
                  // g_play_stat = WAIT; 
		  move(LINES-1,COLS-1);
		  refresh();
                  return ;
                }
		move(LINES-1,COLS-1);
		refresh();
	}
	signal( SIGALRM, ball_move);		/* for unreliable systems */
        return ;
}

int bounce_or_lose(struct ppball *bp)
{
	int return_val = 0 ;
        int bot_row = TOP_ROW + g_net_height;

	if ( bp->y_pos == TOP_ROW+1 ){
		bp->y_dir = 1 ; 
		return_val = 1 ;
                // rand_speed_y();
                bounce_speed_y(); 
	} else if ( bp->y_pos == /*BOT_ROW-1*/ bot_row -1 ){
		bp->y_dir = -1 ;
	       	return_val = 1;
                // rand_speed_y();
                bounce_speed_y(); 
	}

	if ( bp->x_pos == LEFT_EDGE+1 )
        {
            if(g_client_or_server == CLIENT_TTY) // tellBall
            {
  		// bp->x_dir = 1 ;
	       	// return_val = 1 ;
                // bounce_speed_x();
                
              if(bp->x_dir < 0) // go to switch 
                return_val = SWITCH; 
              else
                return_val = NO_BOUNCE;
                
                // tellBALL(bp->y_pos, bp->x_ttm, bp->y_ttm, bp->y_dir);
                // g_play_stat = WAIT; 

            }else if(g_client_or_server == SERVER_TTY)// server check if paddle_contact
            {
              if(paddle_contact(bp->y_pos, bp->x_pos))
              { 
                bp->x_dir = 1; 
                return_val = BOUNCE;
                bounce_speed_x(); 
              }
              else
                return_val = MISSING; 
            }
	} else if ( bp->x_pos == RIGHT_EDGE-1 )
        {
            if(g_client_or_server == SERVER_TTY)
            {
                if(bp->x_dir > 0) // go to switch 
                  return_val = SWITCH; 
                else
                  return_val = NO_BOUNCE;
              // return_val = SWITCH; 
            }else if(g_client_or_server == CLIENT_TTY)
            {
                // check whether the ball contacts with the paddle
	       	// return_val = 1;
                if(paddle_contact(bp->y_pos, bp->x_pos))
                {
	          bp->x_dir = -1;
                  return_val = BOUNCE;
                  bounce_speed_x(); 
                }
                else
                  return_val = MISSING;
            }
	}

	return return_val;
}


void wrap_up()
{

	set_ticker( 0 );
	endwin();		/* put back to normal	*/
}




