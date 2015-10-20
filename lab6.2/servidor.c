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

#define LISTENQ 10
#define MAXDATASIZE 1024

int Socket(int family, int type, int flags)
{
  int sockfd;
  if ((sockfd = socket(family, type, flags)) < 0) {
    perror("socket");
    exit(1);
  } else
    return sockfd;
}
void Bind(int sockfd, struct sockaddr_in servaddr)
{
  if (bind(sockfd, (struct sockaddr *)
	   &servaddr, sizeof(servaddr)) == -1) {
    perror("bind");
    exit(1);
  }
}

void Listen(int listenfd, int flags)
{
  if (listen(listenfd, flags) == -1) {
      perror("listen");
      exit(1);
   }
}

int Accept(int listenfd, struct sockaddr_in *client)
{
  int connfd;
  socklen_t client_size = sizeof(*client);
  if ((connfd = accept(listenfd, (struct sockaddr *) client, &client_size)) == -1 ) {
         perror("accept");
         exit(1);
      }
  return connfd;
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
   Bind(listenfd,servaddr);

   /* O servidor passa a receber conexões */
   Listen(listenfd,LISTENQ);

   for ( ; ; ) {
     struct sockaddr_in client;
     connfd = Accept(listenfd,client);
     
     char client_address[128];
     int client_port = ntohs(client.sin_port);
     inet_ntop(AF_INET,&client.sin_addr.s_addr,client_address,128);
     printf("Serving client %s:%d\n",client_address,client_port);

      int chid = fork();
      if (chid == 0) {
	
	char command[1025];
	int n;
	while(1) {
	  n = read(connfd,command,1024);
	  printf("%d\n",n);
	  if (n==0) {
	    break;
	  }
	  command[n] = 0;
	  //Thats a security flaw, try to chroot this server
	  system(command);
	  write(connfd, command, n);
	}
	close(connfd);
	exit(0);
      } else {
	close(connfd);
      }
   }
   return(0);
}
