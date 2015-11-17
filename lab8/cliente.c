#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "netutils.h"
#define MAXLINE 4096

struct lines {
  char line[MAXLINE];
  struct lines *next;
}

int main(int argc, char **argv) {
   int    sockfd, n;
   char   recvline[MAXLINE + 1];
   char   error[MAXLINE + 1];
   struct sockaddr_in servaddr;
   time_t rawtime;
   struct tm * timeinfo;

   if (argc != 3){
      strcpy(error,"uso: ");
      strcat(error,argv[0]);
      strcat(error," <IPaddress> <Port Number>");
      perror(error);
      exit(1);
   }
   int pnumber;
   sscanf(argv[2],"%d",&pnumber);
   /* Abre o socket */
   sockfd = Socket(AF_INET, SOCK_STREAM, 0);

   /* Prepara as estruturas que serão usadas */
   bzero(&servaddr, sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_port   = htons(pnumber);

   /* Aqui é criada a estrutura que será usada para a conexão */
   Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

   /* A conexão é feita */
   Connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
   time(&rawtime);
   timeinfo = localtime(&rawtime);
   /* printf("%s: connected to %s:%d\n",asctime (timeinfo),argv[1],pnumber); */

   struct lines buffer;
   buffer.next = NULL;
   while (1) {
     /* Find first open position in buffer */
     struct lines *tmp = &buffer;
     while (tmp->next != NULL) tmp = tmp->next;
     
     /*Create fd sets */
     fd_set readfds, writefds;
     FD_ZERO(&readfds);
     FD_ZERO(&writefds);

     FD_SET(sockfd, &readfds);
     FD_SET(sockfs, &writefds);

     int r = select(3, &readfds, &writefds, NULL, NULL);
     if (r == -1) {
       break;
     }
     /* Writing it to server */
     if (FD_ISSET(sockfd,readfds
     Write(sockfd,buffer,strlen(buffer));
     Reading from server
     n = Read(sockfd, buffer, MAXLINE);
     buffer[n+1] = 0;
     fputs(buffer, stdout);
   }
   close(sockfd);
   exit(0);
}
