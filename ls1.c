/*
 *  ls1.c
 *  purpose list contents of directory or directories action if no args, use. 
 *  else list files in args
 *  
 *  Feb. 17, David Z, 
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
#include <malloc.h>
#include <sys/ioctl.h>

void do_ls(char []);
int main(int argc, char* argv[])
{
  if(argc == 1)
    do_ls(".");
  else
  {
    while(--argc)
    {
      // printf("%s:\n", *(++argv));
      do_ls( *(++argv));
    }
  }
  return 0;
}

void qsort(char**, int d[], int st, int et);
char** add_entry(char*);
void print(char**, int n);
void print_in(char** , int* in, int n);
void get_len(char**, int n, int l[], int* max_l, int* min_l);
bool fit_current_ws(int* in, int *l, int n, int *nc_p, int nc, int total_c);  
void display_nc(char** list, int* in, int n, int* l, int* nc_p, int nc);

/*
 * list files in directory called dirname
 * */
void do_ls(char dirname[])
{
  DIR * dir_ptr;           /* directory */
  struct dirent * direntp; /* each entry */
  int i;
  if((dir_ptr = opendir(dirname)) == NULL)
  {
    fprintf(stderr, "ls2: cannot open %s\n", dirname);
  }else
  {
    char ** pdir_files ;
    int n = 0;
    while((direntp = readdir(dir_ptr))!=NULL)
    {
      if(strcmp(direntp->d_name,".") == 0 || 
          strcmp(direntp->d_name, "..") == 0 || 
          direntp->d_name[0]=='.')
        continue;
      // printf("%s\n", direntp->d_name);
      pdir_files = add_entry(direntp->d_name);
      ++n; 
    }

    // print(pdir_files, n);
  
    // sort it 
    int * in = (int*)malloc(n*sizeof(int)); 
    for(i=0; i<n; i++) in[i] = i;
    qsort(pdir_files, in, 0, n);
    // print_in(pdir_files, in, n);
    
    // get length 
    int * l = (int*)malloc(n*sizeof(int));
    int max_l, min_l; 
    get_len(pdir_files, n, l, &max_l, &min_l); 

    // estimate the boundary of the number of columns
    int max_col, min_col; 
    struct winsize w; 
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    max_col = w.ws_col / (min_l); 
    min_col = w.ws_col / (max_l);

    // traverse to findout how many columns fit the current window size 
    int nc;
    int *nc_p = (int*)malloc(max_col*sizeof(int));  // point to the width of each column
    for(nc = max_col; nc >= min_col; --nc)
    {
      if(fit_current_ws(in, l, n, nc_p, nc, w.ws_col)) // got it
      {
        // printf("found suitable number of column: %d\n", nc);
        break;
      }else{
        // printf("nc = %d not fit \n", nc);
      }
    }

    // display according to the nc 
    display_nc(pdir_files, in, n, l, nc_p, nc);

    closedir(dir_ptr);
    
    // allocate memory 
    free(in); free(l); free(nc_p); 
    for(i = 0; i<n; i++)
      free(pdir_files[i]);
    free(pdir_files);
  }
}

void display_nc(char** list, int* in, int n, int* l, int* nc_p, int nc)
{
  int row = n/nc; 
  int rest = n - row*nc; 
  char buf[128] = {0};
  int i,c;
  // for(c=0; c<nc; c++)
    // printf("col %d = %d \n", c, nc_p[c]);
  for(i=0; i<row; i++)
  {
    for(c=0; c<nc; c++)
    {
      char buf[128] = {0};
      if(c<rest)
      {
        printf("%-*.*s", nc_p[c],nc_p[c], list[in[c*(row+1) + i]]);
      }
      else
        printf("%-*.*s", nc_p[c],nc_p[c], list[in[rest*(row+1) + (c-rest)*row + i]]);
    }
    printf("\n");
  }
  for(i=0; i<rest; i++)
  {
    printf("%-*.*s",nc_p[i], nc_p[i], list[in[row+i*(row+1)]]);
  }
  if(rest > 0) printf("\n");
  return ;
}

int max_w(int *l, int* in,  int st, int et)
{
  int ret = -1;
  int i;
  for(i=st; i<et; i++)
  {
    if(l[in[i]] > ret) ret = l[in[i]];
  }
  return ret;
}

// whether the current number of column fit 
bool fit_current_ws(int* in, int *l, int n, int *nc_p, int nc, int total_c)
{
  int row = n/nc; 
  int rest = n - row*nc; 
  int c ;
  int sum_c = 0;
  int cur_i = 0;
  for(c = 0 ; c<nc; c++)
  {
    if(c<rest)
    {
      nc_p[c] = max_w(l, in, cur_i, cur_i + row+1);
      cur_i +=  row + 1;
    }
    else
    {
      nc_p[c] = max_w(l, in, cur_i, cur_i + row); 
      cur_i += row;
    }
    sum_c += nc_p[c]; 
    
    // if(nc == 8) printf(" nc_p[%d] = %d , sum_c = %d, total_c = %d \n", c, nc_p[c], sum_c, total_c);

    if(sum_c > total_c) 
      return false;
  }
  return true;
}

void get_len(char** list, int n, int l[], int* max_l, int* min_l)
{
  if(n <=0) return; 
  int i=0; 
  int len = strlen(list[i]); 
  *max_l = *min_l = len + 2; 
  l[i] = len+2; 
  for(i=1; i<n; i++)
  {
    len = strlen(list[i])+2; // 2 is the space between two columns  
    if(len > *max_l) *max_l = len;
    if(len < *min_l) *min_l = len;
    l[i] = len;
  }
  return ;
}

void swap(int d[], int i1, int i2)
{
  if(i1==i2) return;
  int tmp = d[i1];
  d[i1] = d[i2]; 
  d[i2] = tmp;
  return ;
}

void qsort(char** list, int d[], int st, int et)
{
  if(st >= et)
    return ;

  // partition 
  char* pk = list[d[st]]; // the chosen guard: pivot
  int i, j;
  for(i = st+1, j = et; i<j;)
  {
    if(strcmp(list[d[i]], pk) > 0) // i > pivot
    {
      swap(d, i, j-1);  
      j--;
    }else
      i++;
  }
  // every time i = j, loop stops, where d[i-1] <= pivot and d[i] > pivot
  swap(d, st, i-1); 

  // recursive qsort
  qsort(list, d, st, i-1); 
  qsort(list, d, i, et);
  return ;
}

void print_in(char** list, int * in, int n)
{
  int i; 
  for(i=0; i<n; i++)
  {
    printf("%s \n", list[in[i]]);
  }
}

void print(char** list, int n)
{
  if(n < 0 ) return;
  while(n--)
  {
    printf("%s \n", (*list++));
  }
}

char** add_entry(char* filename)
{
  const static int BLK = 4096; 
  static char ** ret_list = 0; 
  static int n_blk = 0; 
  static int c_ind = 0; 
  // allocate memeory to hold this entry 
  if(c_ind == 0)
  {
    ret_list = (char**)realloc(ret_list, BLK*(n_blk+1)*sizeof(char*));
    if(ret_list == NULL)
    {
      fprintf(stderr, "not enough space to allocate %ld chars\n", BLK*(n_blk+1));
      exit(1);
    }
  }
  // add this entry 
  int len = strlen(filename)+1; 
  char * tmp = (char*)calloc(1, len*sizeof(char));
  strcpy(tmp, filename); 
  ret_list[n_blk*BLK + c_ind] = tmp; 
  
  // this block has been consumed 
  if(++c_ind == BLK)
  {
    c_ind = 0; // start a new block
    ++n_blk ; 
  }
  return ret_list; 
}


