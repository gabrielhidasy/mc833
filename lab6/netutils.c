#include "netutils.h"
int Socket(int family, int type, int flags)
{
  int sockfd;
  if ((sockfd = socket(family, type, flags)) < 0) {
    perror("socket");
    exit(1);
  } else
    return sockfd;
}
void Bind(int sockfd, struct sockaddr *servaddr, int size)
{
  if (bind(sockfd,
	   servaddr, size) == -1) {
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

int Read(int connfd, char *command, int size) {
  int n;
  n = read(connfd,command,size);
  if (n < 0) {
    perror("read");
  }
  return n;
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
void Write(int connfd, char *command, int n)
{
  if(write(connfd, command, n) < 0) {
    perror("write");
  }
}
void Connect(int sockfd, struct sockaddr *servaddr, int size)
{
  if (connect(sockfd, servaddr, size) < 0) {
      perror("connect");
      exit(1);
   }
}
void Inet_pton(int family, char *in, struct in_addr *servaddr)
{
  if (inet_pton(family, in, servaddr) <= 0) {
    perror("inet_pton error");
    exit(1);
  }
}
