/*
 *  who2.c - read /etc/utmp and list info therein
 *         - suppresses empty records
 *         - formats time nicely 
 *
 *        now it can handle "who2 am i"
 * */

#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <utmp.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <stdbool.h>

#define SHOWHOST 

void showtime(long); 
void show_info(struct utmp*); 

int main(int argc, char * argv[])
{
  struct utmp utbuf; /* read info into here */
  int utmpfd; /* read from this descriptor */
  bool b_who_am_i = false;

  if((utmpfd = open(UTMP_FILE, O_RDONLY)) == -1)
  {
    perror(UTMP_FILE); 
    exit(1); 
  }
  
  uid_t effective_uid = -1; 
  char * login_user = getlogin();
  if(argc >= 3 && strcmp(argv[1], "am") == 0 && strcmp(argv[2], "i") == 0)
  {
    printf("who2.c: who am i cmd login user %s\n", login_user); 
    b_who_am_i = true; 
    // effective_uid = geteuid();
    // effective_uid = getuid();
    // printf("effective_uid = %d", effective_uid); 
    // seteuid(effective_uid);
  }
  
  while(read(utmpfd, &utbuf, sizeof(utbuf)) == sizeof(utbuf))
  {
    if(b_who_am_i)
    {
      // printf("who2.c uid = %d effective_uid = %d\n", utbuf.ut_pid, effective_uid);
      // if(utbuf.ut_pid == effective_uid)
      // printf("who2.c current ut name = %s, login_user = %s\n", utbuf.ut_name, login_user);
      if(strcmp(utbuf.ut_name, login_user) == 0)
      {
        printf("%s\n", utbuf.ut_name);
        break;
      }
    }else
    {
      show_info(&utbuf); 
    }
  }
  close(utmpfd);
  return 0;
}
/*
 *  display the content of this utmp struct 
 *  in human readable form 
 *  * display nothing if record has no user name 
 * */
void show_info(struct utmp* utbufp)
{
  if(utbufp->ut_type != USER_PROCESS)
    return ;
  
  printf("% - 8.8s % - 8.8s ", utbufp->ut_name, utbufp->ut_line);  /* log name, and tty*/
  showtime(utbufp->ut_time); 
  
#ifdef SHOWHOST
  if(utbufp->ut_host[0]!='\0')
    printf(" (%s)", utbufp->ut_host); 
#endif
  printf("\n");
}

void showtime(long timeval)
{
  char * cp = ctime(&timeval);  /*string like MON Feb 4 00:46:40 ETS 1991*/ 
  printf("% 12.12s", cp+4); 
}







