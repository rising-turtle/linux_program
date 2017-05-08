#include "bounce.h"

int g_server_quit = 0;
int g_switch_to_server = 0; 
int g_ticks_per_sec = 50; 
int g_net_height = 16; 
int g_client_or_server = CLIENT_TTY;  // 0 client, 1 
int g_play_stat = WAIT; 
float g_pp_version = 1.0; 
char g_player_name[256] = {0};  
char g_pair_name[256] = {0};
char g_result_info[256] = {0}; 
int g_stop_wait = 0;
int g_begin_play = 0;

int g_my_score = 0; 
int g_pair_score = 0; 
