#include <stdio.h>
#include <stdlib.h>
#include "socklib.c"

#define oops(m,x) {perror(m); exit(x);}

extern int g_switch_to_server;
extern int g_server_quit; 
void server_mode(int port); 
void client_mode(char* ip_addr, int port);

// int make_server_socket( int portnum ); 
// int connect_to_server(char* host_name, int portnum);

void request_pp_server(int fd, int port); // pp called in server model 
void request_pp_client(int fd, int port); // pp called in client model

int main(int argc, char* argv[])
{
  if(argc == 2)
  {
    // server mode
    int port_id = atoi(argv[1]); 
    server_mode(port_id); 

  }else if(argc == 3)
  {
    // client mode 
    int port_id = atoi(argv[2]); 
    client_mode(argv[1], port_id);
  }else
  {
    printf("usage: SERVER: net_main PORT; CLIENT: net_main IP_ADDR PORT\n"); 
  }
    
  return 0; 
}

void server_mode(int port_id)
{
  int sock, fd; 
  sock = make_server_socket(port_id); 
  if(sock == -1)
    oops("server sock = -1 ", 1); 
  while(!g_server_quit)
  {
    printf("listen to port %d\n", port_id);
    fd = accept(sock, NULL, NULL); 
    if(fd == -1)
      break; 
    printf("net_main.c: succeed to listen to a client\n"); 
    request_pp_server(fd, port_id);
    // request_pp_client(fd, port_id); 
    close(fd); 
  }
  printf("net_main.c: main exist! \n");
}


void client_mode(char* host_name, int port_id)
{
  printf("net_main.c: try to connect to host:%s port:%d\n", host_name, port_id);
  int fd = connect_to_server(host_name, port_id); 
  if(fd == -1)
    oops("client fd = -1 ", 1); 
  printf("net_main.c: succeed to connect to server  fd = %d\n", fd); 

  request_pp_client(fd, port_id);
  if(g_switch_to_server)
  {
    printf("net_main.c: client switch to server with new port %d!\n", port_id + 1); 
    fflush(stdout);
    server_mode(port_id + 1);
  }
  // request_pp_server(fd, port_id);

  close(fd); 
}

