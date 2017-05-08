#ifndef CONN_REPLY_H
#define CONN_REPLY_H
#include <stdio.h>
#include <stdlib.h>

#define	MAXMSG		512	/* message length max length*/
// #define WAIT            2
// #define PLAY            1

extern FILE* g_fid ;    // for fget/fread/fwrite
extern int g_fd;        // for read/write
extern int g_play_stat; // currect play status

extern int SCgetstr( FILE* infp, char *buf, int len ); 
extern int SCtell(int pop_fd, char* str);  //  

extern int tellHELO(float v, int t_per_sec, int height, char* name);
extern int tellNAME(float v, char* name); 
extern int tellSERV(int n_of_PPs); 
extern int tellBALL(int net_position, int xttm, int yttm, int ydir); 
extern int tellQUIT(char* word); 
extern int tellDONE(char* word); 
extern int tellMISS(); 


#endif
