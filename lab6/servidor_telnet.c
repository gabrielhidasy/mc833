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

FILE *logfd;
int listenfd;

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

void timestamp(char *t)
{
     time_t rawtime;
     struct tm *timeinfo;
     time(&rawtime);
     timeinfo = localtime(&rawtime);
     strcpy(t,asctime(timeinfo));
     t[24] = 0; //TODO, ensure \n is always at 24 or find it
}

void add_client(pid_t pid, char *address, int port)
{
     printf("Adding PID %d\n",pid);
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
     clist->next = NULL;
     clist->c.port = port;
     strcpy(clist->c.address,address);
     clist->c.pid = pid;
     clist = head;
}

int remove_client(pid_t r, struct client *c)
{
     if (r == -1) {
	  return 0;
     }
     printf("Removing PID %d\n",r);
     struct client_list *curr = clist->next;
     struct client_list *prev = clist;
     while (curr->c.pid != r) {
	  curr = curr->next;
	  prev = prev->next;
	  if (curr == NULL)
	       return 0;
     }
     c->port = curr->c.port;
     c->pid = curr->c.pid;
     strcpy(c->address,curr->c.address);
     prev->next = curr->next;
     free(curr);
     return 1;
}

void deal_with_client_chdhandler(int signal)
{
     /* Registering a sighandler for sigchd to interrupt reads when there is no
	output from the process */
     pid_t pid;
     pid = wait(NULL);
     printf("Child %d is dead\n",signal);
}

void deal_with_client(int connfd, char *address, int port)
{
     signal(SIGCHLD,deal_with_client_chdhandler);
     char time_now[25];
     timestamp(time_now);
     printf("%s: connected %s:%d\n",time_now,address,port);
     char command[MAXDATASIZE];
     int n;
     while(1) {
	  int i;
	  for(i = 0; i < MAXDATASIZE; i++) {
	       command[i] = 0;
	  }
	  n = Read(connfd,command,MAXDATASIZE-1);
	  if (n == 0) {
	       break;
	  }
	  command[n] = 0;
	  printf("%s:%d > %s\n",address,port,command);
    

	  char output[MAXDATASIZE];
	  FILE *process_output;
	  process_output = popen(command,"r");
	  n = fread(output,1,MAXDATASIZE,process_output);
	  pclose(process_output);
	  /*When the process ends without output (ex, echo $i > a.txt)*/
	  if (n == 0) {
	       Write(connfd,"\n",2);
	       continue;
	  }
	  Write(connfd,output,n);
     }
     timestamp(time_now);
     printf("%s: disconnected %s:%d\n",time_now,address,port);
     close(connfd);
     exit(0);
}

void chdhandler(int signal)
{
     pid_t pid;
     pid = wait(NULL);
     struct client c;
     if(remove_client(pid,&c)) {
	  char localtime[25];
	  timestamp(localtime);
	  fprintf(logfd,"%s: disconnected %s:%d\n",
		  localtime,c.address,c.port);
     }
}

void inthandler(int signal)
{
     struct client_list *next = clist;
     while (next != NULL) {
	  next = clist->next;
	  free(clist);
	  clist = next;
     }
     fclose(logfd);
     close(listenfd);
     exit(0);
}

int main (int argc, char **argv)
{
     signal(SIGCHLD,chdhandler);
     signal(SIGINT,inthandler);
     int connfd;
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
	  /* This is a classic fork/branch server */
	  int chid = fork();
	  if (chid == 0) {
	       deal_with_client(connfd,client_address,client_port);
	  } else {
	       char localtime[25];
	       timestamp(localtime);
	       /* The parent is always resposable for logs */
	       fprintf(logfd,"%s: connected %s:%d\n",
		       localtime,client_address,client_port);
	       add_client(chid,client_address,client_port);
	       close(connfd);
	  }
     }
     return(0);
}
