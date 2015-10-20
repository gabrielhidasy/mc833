#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "netutils.h"
#include <time.h>
#define LISTENQ 10
#define MAXDATASIZE 4096

struct client {
  int port;
  pid_t pid;
  char address[32];
};

struct client_list {
  struct client c;
  struct client_list *next;
};
  
struct client_list *clist = NULL;
FILE *logfd;

void add_client(pid_t pid, char *address, int port)
{
  if(clist == NULL) {
    clist = malloc(sizeof(struct client_list));
    clist->next = NULL;
  }
  struct client_list *head = clist;
  struct client_list *prev = clist;

  head = clist;
  while (clist != NULL) {
    prev = clist;
    clist = clist->next;
  }
  clist = malloc(sizeof(struct client_list));
  prev->next = clist;
  clist->next == NULL;
  clist->c.port = port;
  strcpy(clist->c.address,address);
  clist->c.pid = pid;
  clist = head;
}

void remove_client(pid_t r, struct client *c)
{
  struct client_list *curr = clist->next;
  struct client_list *prev = clist;
  printf("%d %d\n",prev,r);
  while (curr->c.pid != r) {
    curr = curr->next;
    prev = prev->next;
    }
  c->port = curr->c.port;
  c->pid = curr->c.pid;
  strcpy(c->address,curr->c.address);
  prev->next = curr->next;
  free(curr);
}
    

void deal_with_client(int connfd, char *address, int port)
{
  time_t rawtime;
  struct tm * timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  printf("%s: connected %s:%d\n",asctime (timeinfo),address,port);
  char command[MAXDATASIZE];
  int n;
  while(1) {
    n = Read(connfd,command,MAXDATASIZE-1);
    if (n == 0) {
      break;
    }
    printf("%d\n",n);
    command[n] = 0;
    printf("%s:%d > %s\n",address,port,command);
    //Thats a security flaw, try to chroot this server
    //system(command);
    Write(connfd,command,n);
  }
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  printf("%s: disconnected %s:%d\n",asctime (timeinfo),address,port);
  close(connfd);
  exit(0);
}

void sighandler(int signal)
{
  pid_t pid;
  pid = wait(NULL);
  struct client c;
  remove_client(pid,&c);
  time_t rawtime;
  struct tm *timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  char localtime[25];
  strcpy(localtime,asctime(timeinfo));
  localtime[24] = 0; //TODO, ensure \n is always at 24 or find it
  fprintf(logfd,"%s: connected %s:%d\n",
	  localtime,c.address,c.port);
}

int main (int argc, char **argv)
{
  signal(SIGCHLD,sighandler);
  int    listenfd, connfd;
  struct sockaddr_in servaddr;
  int pnumber = 12344;

  //Adicionar uma sentinela a lista de clientes
  add_client(-1,"-1",-1);
  if (argc == 2) {
    sscanf(argv[1],"%d",&pnumber);
  }
  logfd = fopen("logfile.log","w");
  /* No trecho abaixo um socket é inicializado */
  listenfd = Socket(AF_INET, SOCK_STREAM, 0);
  
  /* Estruturas são preparadas */
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family      = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port        = htons(pnumber);   

  /* A porta é reservada */
  Bind(listenfd,(struct sockaddr *) &servaddr, sizeof(servaddr));

  /* O servidor passa a receber conexões */
  Listen(listenfd,LISTENQ);

  for ( ; ; ) {
    struct sockaddr_in client;
    connfd = Accept(listenfd,&client);
    char client_address[32];
    int client_port = ntohs(client.sin_port);
    inet_ntop(AF_INET,&client.sin_addr.s_addr,client_address,128);
    int chid = fork();
    if (chid == 0) {
      deal_with_client(connfd,client_address,client_port);
    } else {
      time_t rawtime;
      struct tm * timeinfo;
      time(&rawtime);
      timeinfo = localtime(&rawtime);
      char localtime[25];
      strcpy(localtime,asctime(timeinfo));
      localtime[24] = 0; //TODO, ensure \n is always at 24 or find it
      fprintf(logfd,"%s: connected %s:%d\n",
	      localtime,client_address,client_port);
      add_client(chid,client_address,client_port);
      close(connfd);
    }
  }
  return(0);
}
