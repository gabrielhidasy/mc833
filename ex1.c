/***********************************************************/
/* Gabriel Hidasy Rezende, RA116928 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include<arpa/inet.h>
#include<signal.h>
void err(char *msg)
{
  fprintf(stderr,"%s\n",msg);
  exit(0);
}

int stopsignal = 1;
void sighandler(int signo)
{
  if (signo == SIGINT) {
    printf("Server shutting down, this can take a minute...\n");
    stopsignal = 0;
  }
}

/* Exercicio 1a, um exemplo de codigo para um servidor TCP */

void TCPserverAccepter(int client)
{
  send(client,"200 OK\nHI\n",10,0);
  close(client);
}
void TCPserverListener()
{
  int sockfd, client, port = 12345;
  char buffer[1024];
  struct sockaddr_in serv_addr, cli_addr;
  sockfd = socket(AF_INET, SOCK_STREAM,0);
  if(sockfd < 0) {
    err("Erro abrindo o socket TCP de servidor");
  }
  //Ler sobre os campos dessa estrutura
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(port);
  
  int len = sizeof(serv_addr);

  if(bind(sockfd, (struct sockaddr *) &serv_addr, len) < 0) {
    err("Erro no binding do servidor TCP");
  }
  listen(sockfd,5);

  int pid;
  while (stopsignal) {
    client = accept(sockfd, (struct sockaddr *) &cli_addr, &len);
    pid = fork();
    if (pid < 0) {
      close(client);
      err("Erro no fork()");
      continue;
    }
    if (pid == 0) {
      /*Trata o cliente*/
      TCPserverAccepter(client);
      break;
    }
    /*Loga o pid do filho pra matar o servidor de forma limpa com um C-c*/
  }
  close(sockfd);
  return;
}

/* Exercicio 1b, um exemplo de codigo para um cliente TCP */
void TCPClient() {
  struct sockaddr_in my_addr, server_addr;
  int sockfd;
  sockfd = socket(AF_INET,SOCK_STREAM,0);
  memset(&my_addr,0,sizeof(my_addr));
  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons(12345);
  my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  if (sockfd < 0) {
    err("Erro com o socket do cliente TCP");
  }
  int len = sizeof(my_addr);
  if(connect(sockfd, (struct sockaddr*) &my_addr, len) < 0) {
    err("Erro na conect do cliente TCP");
  }
  fprintf(stdout,"Cliente conectado!\n");
  char buffer[1024];
  recv(sockfd,&buffer,1024,0);
  fprintf(stdout,"from the server:\n%s",buffer);
  close(sockfd);
  return;    
}

void UDPServer()
{
  int sockfd, client, port = 12345;
  char buffer[1024];
  struct sockaddr_in serv_addr, cli_addr;
  sockfd = socket(AF_INET, SOCK_DGRAM,0);
  if(sockfd < 0) {
    err("Erro abrindo o socket UDP de servidor");
  }
  //Ler sobre os campos dessa estrutura
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(port);
  
  int len = sizeof(serv_addr);

  if(bind(sockfd, (struct sockaddr *) &serv_addr, len) < 0) {
    err("Erro no binding do servidor UDP");
  }
  /* O servidor UDP não usa listen(n) */

  int pid;
  int rlen;
  while (stopsignal) {
    /* Logo ele também não roda accept, só recv */
    rlen = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr *) &cli_addr, &len);
    buffer[rlen] = 0;
    printf("%s\n",buffer);
    sendto(sockfd,buffer,rlen,0,(struct sockaddr *) &cli_addr,sizeof(cli_addr));
    /* E como não há conexão, o socket não fica preso a um cliente, */
    /*o que diminue  muito a necessidade de forks ou threads */
  }
  close(sockfd);
  return;
}

/* Exercicio 2b, um exemplo de codigo para um cliente UDP */
void UDPClient() {
  /* UDP é um pouco diferente de TCP, aqui não há coneção*/
  struct sockaddr_in server_addr;
  int sockfd;
  /* A primeira diferença é que o socket é de datagram */
  sockfd = socket(AF_INET,SOCK_DGRAM,0);
  memset(&server_addr,0,sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(12345);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  if (sockfd < 0) {
    err("Erro com o socket do cliente UDP");
  }
  int len = sizeof(server_addr);
  char sbuffer[1024] = "mensagem do cliente pra você\nola\n";
  char rbuffer[1024];
  int rlen;
  sendto(sockfd,sbuffer,strlen(sbuffer),0,(struct sockaddr *) &server_addr, sizeof(server_addr));
  rlen = recvfrom(sockfd,rbuffer,1024,0,NULL,NULL);
  rbuffer[rlen] = 0;
  fprintf(stdout,"from the server:\n%s",rbuffer);
  close(sockfd);
  return;    
}

int main (int argc, char *argv[])
{
  signal(SIGINT,sighandler);
  //TCPserverListener();
  //TCPClient();
  //UDPClient();
  UDPServer();
}

