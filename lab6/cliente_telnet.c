#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "netutils.h"
#define MAXLINE 4096

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
     printf("%s: connected to %s:%d\n",asctime (timeinfo),argv[1],pnumber);

     char command[MAXLINE];
     while (1) {
	  /* Le a linha de comando */
	  n = scanf(" %[^\n]",command);
	  if (n <= 0) {
	       break;
	  }
	  if (!strcmp("Bye",command)) {
	       break;
	  }
	  /* Envia o comando */
	  Write(sockfd,command,strlen(command));
	  /* Recebe a resposta */
	  n = Read(sockfd, recvline, MAXLINE);
	  if (n == 0) {
	       break;
	  }
	  recvline[n] = 0;
	  printf("%s\n",recvline);
     }
     close(sockfd);
     exit(0);
}
