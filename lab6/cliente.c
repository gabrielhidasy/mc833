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

#define MAXLINE 4096

int main(int argc, char **argv) {
   int    sockfd, n;
   char   recvline[MAXLINE + 1];
   char   error[MAXLINE + 1];
   struct sockaddr_in servaddr;

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
   if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      perror("socket error");
      exit(1);
   }

   /* Prepara as estruturas que serão usadas */
   bzero(&servaddr, sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_port   = htons(pnumber);

   /* Aqui é criada a estrutura que será usada para a conexão */
   if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
      perror("inet_pton error");
      exit(1);
   }

   /* A conexão é feita */
   if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
      perror("connect error");
      exit(1);
   }
   printf("Connected to %s:%d\n",argv[1],pnumber);

   char command[1025];
   while (1) {
     /* Le a linha de comando */
     n = scanf(" %[^\n]",command);
     if (n == 0) {
       break;
     }
     /* Envia o comando */
     write(sockfd,command,strlen(command));
     /* Recebe a resposta */
     n = read(sockfd, recvline, MAXLINE);
     if (n == 0) {
       break;
     }
     recvline[n] = 0;
     printf("%s\n",recvline);
     }
   exit(0);
}
