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
};

int main(int argc, char **argv) {
   int    sockfd, n;
   struct sockaddr_in servaddr;
   time_t rawtime;
   struct tm * timeinfo;
   char error[100];
   
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

   struct timeval timeout;
   timeout.tv_sec = 0;
   timeout.tv_usec = 10000;
   struct lines buffer;
   char line[MAXLINE];
   int blocks = 0;
   void *stdinEOF = (void *) 1;
   buffer.next = NULL;
   while (1) {
     /* Find first open position in buffer */
     struct lines *tmp = &buffer;
     while (tmp->next != NULL) tmp = tmp->next;
     if (stdinEOF) {
       stdinEOF = fgets(tmp->line,MAXLINE,stdin);
       blocks++;
       //fprintf(stderr, "Read a line, blocks = %d\n",blocks);
     }
     /*Create fd sets */
     fd_set readfds, writefds;
     FD_ZERO(&readfds);
     FD_ZERO(&writefds);

     /* If theres something to write pool read and write */
     if (blocks > 0) {
       FD_SET(sockfd, &readfds);
       FD_SET(sockfd, &writefds);
     } else {
       FD_SET(sockfd, &readfds);
     }
     int r = select(3, &readfds, &writefds, NULL, &timeout);
     if (r == -1) {
       break;
     }
     if (FD_ISSET(sockfd,&writefds) || 1) {
       fprintf(stderr, "Will write\n");
       Write(sockfd,buffer.line,strlen(buffer.line));
       if (buffer.next == NULL) {
	 buffer.next = malloc(sizeof(struct lines));
       }
       buffer = *(buffer.next);
       blocks--;
     }
     if (blocks == 0 && !stdinEOF) {
       break;
     }
     if (FD_ISSET(sockfd,&readfds) || 1) {
       n = Read(sockfd, line, MAXLINE);
       line[n+1] = 0;
       fputs(line, stdout);
     }
   }
   close(sockfd);
   exit(0);
}
