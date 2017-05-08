#include "conn_reply.h"
#include <string.h>

FILE* g_fid = 0; 
int g_fd = 0;

int tellHELO(float v, int t_per_sec, int height, char* name)
{
  char buf[MAXMSG] = {0};
  sprintf(buf, "HELO %f %d %d %s", v, t_per_sec, height, name); 
  return SCtell(g_fd, buf); 
}
int tellNAME(float v, char* name)
{
  char buf[MAXMSG] = {0}; 
  sprintf(buf, "NAME %s", name); 
  return SCtell(g_fd, buf); 
}
int tellSERV(int n_of_PPs)
{
  char buf[MAXMSG] = {0};
  sprintf(buf, "SERV %d", n_of_PPs); 
  return SCtell(g_fd, buf);
}
int tellBALL(int net_position, int xttm, int yttm, int ydir)
{
  char buf[MAXMSG] = {0}; 
  sprintf(buf, "BALL %d %d %d %d", net_position, xttm, yttm, ydir); 
  return SCtell(g_fd, buf); 
}
int tellQUIT(char* word)
{
  char buf[MAXMSG] = {0}; 
  sprintf(buf, "QUIT %s", word); 
  return SCtell(g_fd, buf); 
}
int tellDONE(char* word)
{
  char buf[MAXMSG] = {0}; 
  sprintf(buf, "DONE %s", word); 
  return SCtell(g_fd, buf);
}

int tellMISS()
{
  char* miss = "MISS"; 
  // printf("tellMISS: g_fd = %d\n", g_fd);
  return SCtell(g_fd, miss);
}


int SCgetstr( FILE* infp, char *buf, int len )
/*
 * purpose: retrieve one line from server
 *    args: a place to store reply, length of buffer
 * returns: -1 on error (including EOF), 0 if ok
 *   notes: the rfc says reply is <= 512 bytes
 *	    this makes sure the reply ends with CRLF
 * 	    AND removes the CRLF from the line
 *  extra!: this one undoes byte-stuffing (read the RFC)
 */
{
	char	*rv = NULL;

	if( fgets(buf,len,infp) == NULL )		/* EOF ? */
        {
            printf("EOF?\n");
	    return -1;
        }
// #ifdef	CRLF_NEEDED
	if ( strcmp(buf+strlen(buf)-2, "\r\n") != 0 )	/* ends with CRLF? */
        {
            printf("no CRLF?\n");
	    return -1;
        }
// #endif

	if ( *buf == '.' && strlen(buf) > 3 ){		/* byte-stuffed? */
            printf("........?\n");
		char *s = buf+1, *d = buf;
		while(*d++ = *s++) ;
	}
	buf[strlen(buf)-2] = '\0';
	return 0;
}

int SCtell(int pop_fd, char* str)  //  
{
/*
 * purpose: send a mesage to the server
 *     arg: str is a string
 *     ret: -1 for error, 0 for ok
 *   notes: this appends CRLF to the string and sends it
 */
	char	buf[MAXMSG+1] = {0};
	int	len = strlen(str);
	
	if ( len + 3 > MAXMSG )
		return -1;
	sprintf(buf, "%s\r\n", str );
        // printf("pop_fd: %d buf: %s len: %d\n", pop_fd, buf, len);
	if ( write( pop_fd, buf, len+2 ) != len+2 )
        // if(fwrite(buf, len+2, 1, g_fid) != 1)
		return -1;
	return 0;

}




