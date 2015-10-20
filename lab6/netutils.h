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

int Socket(int family, int type, int flags);
void Bind(int sockfd, struct sockaddr_in servaddr);
void Listen(int listenfd, int flags);
int Read(int connfd, char *command, int size);
int Accept(int listenfd, struct sockaddr_in *client);
void Write(int connfd, char *command, int n);
void Connect(int sockfd, struct sockaddr *servaddr, int size);
void Inet_pton(int type, char *in, struct sockaddr_in servaddr);
