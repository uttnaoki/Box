/* $Id: http-client.c,v 1.6 2013/01/23 06:57:19 68user Exp $ */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <arpa/inet.h>

int fd;
char send_buf[1000];
char recv_buf[1000];
int stl;

void check(int x){
  printf("check %d\n",x);
}

void recvbuf(){
  bzero(recv_buf,sizeof(recv_buf));
  if(recv(fd,recv_buf,1000,0)==-1){
    printf("error:recv\n");
    exit(0);
  }
}

void sendbuf(){
  if(send(fd,send_buf,strlen(send_buf),0)==-1){
    printf("error:send\n");
    exit(0);
  }
}

int main(int argc, char *argv[]){
  //int fd;
  struct hostent *host;
  struct sockaddr_in sa;
  //char send_msg[1000];
  //char recv_msg[1000];
  //int stl;

  if((host = gethostbyname("localhost")) == NULL){
    printf("error: gethostbyname\n");
    return 1;
  }

  if((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    printf("error: socket");
    return 1;
  }

  sa.sin_family = host->h_addrtype;
  sa.sin_port = htons(3000);
  bzero((char *)&sa.sin_addr, 12);
  memcpy((char *)&sa.sin_addr, (char *)host->h_addr, host->h_length);

  /*
  sa.sin_family = AF_INET;
  sa.sin_addr.s_addr = htonl(argv[1]);
  sa.sin_port = htons(3000);*/
  
  if(connect(fd, (struct sockaddr *)&sa, sizeof(sa)) == -1){
    printf("error: connect\n");
    return 1;
  }

  
  bzero(send_buf,sizeof(send_buf));

  recvbuf();                                      //ファイル名を入力してください

  while(1){
    printf("%s\n",recv_buf);
    stl = read(0,send_buf,1000);
    sendbuf();

    if(strcmp(send_buf,"q\n")==0) return 0;        //コメントを外せばqで終了可能
  
    recvbuf();                                     // buf = "start\n"
    if(strcmp(recv_buf,"start\n")==0) break;
  }
    printf("\n\n%s\n",recv_buf);

  sprintf(send_buf,"ok\n");
  sendbuf();
  recvbuf();                                     // 1問目
  while(strncmp(recv_buf,"result",6)){
  printf(recv_buf);
  bzero(send_buf,sizeof(send_buf));
  stl=read(0,send_buf,1000);
  sendbuf();
  recvbuf();
  printf(recv_buf);
  sprintf(send_buf,"next\n");
  sendbuf();
  recvbuf();
  }                                             // recvでループを抜ける

  printf(recv_buf);
  
  if(close(fd) == -1){
    printf("error: close\n");
    return 1;
  }

  return 1;
}
