/*
 *  Feb. 26, 2016, David Z
 *
 *  mv2.c mv src to dst/src or dst 
 *
 *  usage: mv2 src dst
 *
 * */

#include <stdio.h>
#include <unistd.h>
#include <utmp.h>
#include <fcntl.h> // this file includes <bits/stat.h> which defined the struct stat. 
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <stdbool.h>

#define BUFFERSIZE 4096
#define COPYMODE   0644 

void oops(char *, char* );

int same_file(int fd1, int fd2);

typedef enum{FILE_TYPE = 1, DIR_TYPE, OTHER_TYPE } F_TYPE;

F_TYPE is_file(int fd);  // 1, file, 0, directory, -1 other 

int main(int argc, char * argv[])
{
  int in_fd, out_fd, n_chars; 
  int in_fd2;
  char buf[BUFFERSIZE]; 
    
  if(argc != 3)
  {
    fprintf(stderr, "usage: %s src dst\n", *argv);
    exit(1);
  }
  
  if((in_fd=open(argv[1], O_RDONLY)) == -1)
    oops("Cannot open", argv[1]);
  
  if((in_fd2=open(argv[2], O_RDONLY)) != -1) // dst exist 
  {
    F_TYPE fd2_type = is_file(in_fd2); 
    if(fd2_type == FILE_TYPE) // dst is a file 
    {
      rename(argv[1], argv[2]); 
    }else if(fd2_type == DIR_TYPE)  // dst is a directory 
    { 
      strcpy(buf, argv[2]);
      strcat(buf, "/"); 
      strcat(buf, argv[1]);
      rename(argv[1], buf); 
    }else
    {
      oops("dst neither file nor directory", argv[2]); 
    }
  }else // dst not exist, must be a file
  {
    if( rename(argv[1], argv[2]) == -1)
    {
      printf(" failed to mv %s to %s\n", argv[1], argv[2]); 
    }
  }
  return 0;
}


F_TYPE is_file(int fd) // 1, file, 0, directory, -1 other 
{
  F_TYPE ret = OTHER_TYPE; 
  struct stat info_p; 
  if(fstat(fd, &info_p) < 0) 
  {
    oops("what? fd cannot be accessed!", "--"); 
  }
  
  if(S_ISDIR(info_p.st_mode))
  {
    ret = DIR_TYPE; 
  }else if(S_ISREG(info_p.st_mode))
  {
    ret = FILE_TYPE; 
  }
  return ret;
}

void oops(char * s1, char* s2)
{
  fprintf(stderr, "Error : %s ", s1); 
  perror(s2); 
  exit(1);
}
