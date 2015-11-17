#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "netutils.h"

#define LISTENQ 10
#define MAXDATASIZE 4096

void deal_with_client(int connfd)
{
  char command[MAXDATASIZE];
  int n;
  while(1) {
    n = Read(connfd,command,MAXDATASIZE-1);
    if (n == 0) {
      break;
    }
    command[n] = 0;
    //Thats a security flaw, try to chroot this server
    //system(command);
    printf("%s\n", command);
    Write(connfd,command,n);
  }
}

int main (int argc, char **argv)
{
  int    listenfd, connfd;
  struct sockaddr_in servaddr;
  int pnumber = 12344;
  if (argc == 2) {
    sscanf(argv[1],"%d",&pnumber);
   }
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
     char client_address[128];
     int client_port = ntohs(client.sin_port);
     inet_ntop(AF_INET,&client.sin_addr.s_addr,client_address,128);
     printf("Serving client %s:%d\n",client_address,client_port);

      int chid = fork();
      if (chid == 0) {
	deal_with_client(connfd);
	close(connfd);
	exit(0);
      } else {
	close(connfd);
      }
   }
   return(0);
}
