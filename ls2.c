/*
 *  ls2.c
 *  purpose list contents of directory or directories action if no args, use. 
 *  else list files in args
 *  note uses stat and pwd.h and grp.h
 *
 *  BUG: try ls2 /tmp
 *  
 *  Feb. 17, David Z
 *
 * */


#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <time.h>

void do_ls(char []);
bool dostat(char *);
bool show_file_info(char*, struct stat*);
bool mode_to_letters(int, char[]);
char * uid_to_name(uid_t); 
char * gid_to_name(gid_t); 

bool recursive_do_ls = false;
char cwd[1024]; 
int main(int argc, char* argv[])
{
  getcwd(cwd, 1024);
  if(argc == 1)
    do_ls(".");
  else
  {
    if(strcmp(argv[1], "-R") == 0)
    {
      recursive_do_ls = true;
      if(argc == 2)
        do_ls(".");
      else
      {
        int i ; 
        for(i=1; i< argc-1; i++)
          do_ls(argv[i+1]);
      }
    }
    else
    {
      while(--argc)
      {
        printf("%s:\n", *(++argv));
        do_ls(*argv);
      }
    }
  }
  return 0;
}

/*
 * list files in directory called dirname
 * */
void do_ls(char dirname[])
{
  DIR * dir_ptr;           /* directory */
  struct dirent * direntp; /* each entry */
  printf("in do_ls input dirname = %s\n", dirname);
  if((dir_ptr = opendir(dirname)) == NULL)
  {
    fprintf(stderr, "ls2: cannot open %s\n", dirname);
  }else
  {
    char** dir_list = (char**)malloc(4096*sizeof(char*));
    bool is_dir; 
    int n_dir = 0;
    while((direntp = readdir(dir_ptr))!=NULL)
    {
      char buf[1024]={0};
      strcpy(buf, cwd);
      strcat(buf, "/");
      strcat(buf, dirname);
      strcat(buf, "/");
      strcat(buf, direntp->d_name);
      // is_dir = dostat(direntp->d_name);
      is_dir = dostat(buf);
      if(recursive_do_ls && is_dir)
      {
        // printf("detect a directory: %s\n", direntp->d_name);
        // int n = strlen(direntp->d_name) + 1; 
        char* tmp = (char*)malloc(256*sizeof(char)); 
        // strcpy(tmp, direntp->d_name); 
        strcpy(tmp, dirname);
        strcat(tmp, "/"); 
        strcat(tmp, direntp->d_name);
        dir_list[n_dir++] = tmp; 
      }
    }
    if(recursive_do_ls)
    {
      int i;
      for(i=0; i< n_dir; i++)
        do_ls(dir_list[i]);
      
      for(i=0; i< n_dir; i++)
        free(dir_list[i]);
    }
    free(dir_list);
    closedir(dir_ptr);
  }
  
  /*
  struct stat info;
  char * pdo = "/home/davidz/work/study/linux_pram/tmp/dir2/mm2.c";
  if(stat(pdo, &info) == -1)
  {
    printf("the same error ");
    perror(pdo); 
  }else{
    printf("succeed to open file: %s", pdo);
  }*/
}

bool dostat(char* filename)
{
  bool is_dir = false;
  struct stat info; 
  if(stat(filename, &info) == -1)
  {
    // printf("failed to open filename: %s\n", filename);
    perror(filename); /*cannot stat*/
  }
  else
  { 
    // printf("succed to open filename: %s\n", filename);
    is_dir = show_file_info(filename, &info);
  }
  return is_dir;
}

bool show_file_info(char* filename, struct stat* info_p)
{
  char modestr[11]; 
  bool is_dir = mode_to_letters(info_p->st_mode, modestr); 
  printf("%s", modestr); 
  printf("%4d ", (int)(info_p->st_nlink));
  printf("%-8s ", uid_to_name(info_p->st_uid)); 
  printf("%-8s ", gid_to_name(info_p->st_gid));
  printf("%8lld ", (long long)(info_p->st_size));
  printf("%.12s ", ctime(&(info_p->st_mtime))+4); 

  // printf("%s\n", filename);
  char * fname = (strrchr(filename, '/')+1);
  // printf("%s\n", filename);
  printf("%s\n", fname);

  // return (is_dir && (strcmp(filename, ".") != 0) && (strcmp(filename, "..") !=0));
  return (is_dir && (strcmp(fname, ".") != 0) && (strcmp(fname, "..") !=0));
}

bool mode_to_letters(int mode, char str[])
{
  bool is_dir = false;
  strcpy(str, "---------- ");
  if(S_ISDIR(mode)) 
  {
    str[0] = 'd'; /* file property, directory, char decive or block device */
    is_dir = true;
  }
  if(S_ISCHR(mode)) str[0] = 'c';
  if(S_ISBLK(mode)) str[0] = 'b';
  
  if(mode & S_IRUSR) str[1] = 'r'; /* 3 bits for user */
  if(mode & S_IWUSR) str[2] = 'w'; 
  if(mode & S_IXUSR) str[3] = 'x';

  if(mode & S_IRGRP) str[4] = 'r'; /* 3 bits for group*/
  if(mode & S_IWGRP) str[5] = 'w'; 
  if(mode & S_IXGRP) str[6] = 'x'; 

  if(mode & S_IROTH) str[7] = 'r'; /* 3 bits for other */
  if(mode & S_IWOTH) str[8] = 'w'; 
  if(mode & S_IXOTH) str[9] = 'x'; 
  return is_dir; 
}


char* uid_to_name(uid_t uid)
{
  struct passwd *pw_ptr; 
  static char numstr[10]; 
  if((pw_ptr = getpwuid(uid)) == NULL)
  {
    sprintf(numstr, "%d", uid);   
    return numstr;
  }
  return pw_ptr->pw_name; 
}


char * gid_to_name(gid_t gid)
{
  struct group* grp_ptr; 
  static char numstr[10]; 

  if((grp_ptr = getgrgid(gid)) == NULL)
  {
    sprintf(numstr, "%d", gid);
    return numstr;
  }
  return grp_ptr->gr_name;
}


