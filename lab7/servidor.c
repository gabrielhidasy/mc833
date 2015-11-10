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

#define MAXDATASIZE 100

int main (int argc, char **argv) {
     int    listenfd, connfd;
     struct sockaddr_in servaddr;
     char   buf[MAXDATASIZE];
     time_t ticks;
     if (argc != 2) {
	  printf("You must specify the size of the backlog\n");
	  exit(0);
     }
     int backlog;
     sscanf(argv[1],"%d",&backlog);
     /* No trecho abaixo um socket é inicializado */
     if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	  perror("socket");
	  exit(1);
     }

     /* Estruturas são preparadas */
     bzero(&servaddr, sizeof(servaddr));
     servaddr.sin_family      = AF_INET;
     servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
     servaddr.sin_port        = htons(12345);   

     /* A porta (12344) é reservada */
     if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
	  perror("bind");
	  exit(1);
     }

     /* O servidor passa a receber conexões */
     if (listen(listenfd, backlog) == -1) {
	  perror("listen");
	  exit(1);
     }

     /* Loop principal do servidor, aqui ele escuta uma conexão e ao receber uma */
     /* retorna a data do dia, note que esse servidor não é multi-thread mas */
     /* ainda funciona bem com varios clientes pois ele mesmo se encarrega de */
     /* fechar a conexão */
     for ( ; ; ) {
	  system("sleep 1024");
	  if ((connfd = accept(listenfd, (struct sockaddr *) NULL, NULL)) == -1 ) {
	       perror("accept");
	       exit(1);
	  }

	  ticks = time(NULL);
	  struct sockaddr_in remote;
	  int len = sizeof(remote);
	  getpeername(connfd, (struct sockaddr *) &remote, &len);
	  printf("Remote socket %s\n",inet_ntoa(remote.sin_addr));

	  /* Aqui a mensagem de resposta é montada */
	  snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));
	  /* E enviada */
	  write(connfd, buf, strlen(buf));
	  /* O socket dessa conexão é terminado e vamos para o proximo cliente */
	  close(connfd);
     }
     return(0);
}
