#ifndef BOUNCE_H
#define BOUNCE_H

/* bounce.h			*/

/* some settings for the game	*/

#define	BLANK		' '
#define	DFL_SYMBOL	'o'
#define	TOP_ROW		4
#define	BOT_ROW 	21
#define	LEFT_EDGE	9
// #define	RIGHT_EDGE	70
#define	RIGHT_EDGE	40
#define	X_INIT		20		/* starting col		*/
#define	Y_INIT		10		/* starting row		*/
#define	TICKS_PER_SEC	50		/* affects speed	*/
#define CLIENT_TTY 1
#define SERVER_TTY 0

extern int g_server_quit;             // whether server quits
extern int g_switch_to_server;        // whether to switch to server
extern char g_player_name[256];       // player name
extern char g_pair_name[256];         // other player name
extern char g_result_info[256];       // save the result
extern float g_pp_version;            // ppball version 
extern int g_ticks_per_sec;           // speed 
extern int g_net_height;              // net height 
extern int g_client_or_server;        // whether client or server
extern int g_stop_wait;               // flags whether to wait
extern int g_begin_play;              // indicates whether start play

extern int g_my_score;      
extern int g_pair_score;  

#define WAIT 3
#define PLAY 4
#define SERV 5
extern int g_play_stat ; 

#define	X_TTM		5
#define	Y_TTM		8
#define X_MAX_TTM       8
#define Y_MAX_TTM       5

#define MISSING         -1
#define BOUNCE          1
#define NO_BOUNCE       0
#define SWITCH          2

/** the ping pong ball **/

struct ppball {
		int	y_pos, x_pos,
			y_ttm, x_ttm,
			y_ttg, x_ttg,
			y_dir, x_dir;
		char	symbol ;

	} ;

#endif
