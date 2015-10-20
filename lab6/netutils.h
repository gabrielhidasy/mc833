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

/* Poderia ter simplificado algumas chamadas, eliminando os int size */
/* fazendo os mesmos dentro dos wrappers, mas do jeito que fiz eles são */
/* compativeis o bastante para serem  usados como drop-in replacements para */
/* as funções de baixo nivel*/

int Socket(int family, int type, int flags);
void Bind(int sockfd, struct sockaddr *servaddr, int size);
void Listen(int listenfd, int flags);
int Read(int connfd, char *command, int size);
int Accept(int listenfd, struct sockaddr_in *client);
void Write(int connfd, char *command, int n);
void Connect(int sockfd, struct sockaddr *servaddr, int size);
void Inet_pton(int type, char *in, struct in_addr *servaddr);
