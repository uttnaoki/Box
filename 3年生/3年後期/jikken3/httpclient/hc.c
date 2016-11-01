/* $Id: http-client.c,v 1.6 2013/01/23 06:57:19 68user Exp $ */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]){
  int fd;
  struct hostent *host;
  struct sockaddr_in sa;
  char send_msg[1024];
  char buf[10000];
  int buf_len;

  if((host = gethostbyname(argv[1])) == NULL){
    printf("error: gethostbyname\n");
    return 1;
  }

  if((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    printf("error: socket");
    return 1;
  }

  sa.sin_family = host->h_addrtype;
  sa.sin_port = htons(80);
  bzero((char *)&sa.sin_addr, 12);
  memcpy((char *)&sa.sin_addr, (char *)host->h_addr, host->h_length);
  
  if(connect(fd, (struct sockaddr *)&sa, sizeof(sa)) == -1){
    printf("error: connect\n");
    return 1;
  }

  strcpy(send_msg, "GET ");
  strcat(send_msg, argv[2]);
  strcat(send_msg, "\n");

  if(send(fd, send_msg, sizeof(send_msg), 0) == -1){
    printf("error: send\n");
    return 1;
  }

  if(recv(fd, buf, sizeof(buf), 0) == -1){
    printf("error: connect\n");
    return 1;
  }

  buf_len = strlen(buf);
  if(write(1, buf, buf_len) == -1){
    printf("error: write\n");
  }

  if(close(fd) == -1){
    printf("error: close\n");
    return 1;
  }

  return 1;
}
