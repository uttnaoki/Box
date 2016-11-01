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
int i=0;
int printflag=0;

FILE *dfpc;

void check(int x){
  printf("check %d\n",x);
}

void sendbuf(){
  if(send(fd,send_buf,strlen(send_buf),0)==-1){
    printf("error:send\n");
    exit(0);
  }
  if(printflag==1){
    fprintf(dfpc,"debugs %s\n",send_buf);
    fprintf(dfpc,"================================\n");
  }
  if(printflag==2){
    printf("=============send===================\n");
    printf("%s\n",send_buf);
    printf("=============send===================\n");
  }
  //bzero(send_buf,sizeof(send_buf));
}

void recvbuf(){
  bzero(recv_buf,sizeof(recv_buf));
  if(recv(fd,recv_buf,1000,0)==-1){
    printf("error:recv\n");
    exit(0);
  }
  if(printflag==1){
    fprintf(dfpc,"debugr %s\n",recv_buf);
    fprintf(dfpc,"********************************\n");
  }
  if(printflag==2){
    printf("************recv*******************\n");
    printf("%s\n",recv_buf);
    printf("************recv*******************\n");
  }
}


int subst(char *str, char c1, char c2){
  int n = 0;
  while(*str){
    if(*str == c1){
      *str = c2;
      n++;
    }
    str++;
  }
  return n;
}

int send_file_data(char *buf){
  FILE *fp;
  
  subst(buf,'\n','\0');
  if((fp = fopen(buf,"r")) == NULL){
    fprintf(stderr,"ファイルがありません\n");
    return 0;
  }
  while(fgets(send_buf,1000,fp)!=NULL){
    sendbuf();
    printf("%s\n",send_buf);
    recvbuf();
  }
  bzero(send_buf,sizeof(send_buf));
  sprintf(send_buf,"EOF");
  sendbuf();
  bzero(send_buf,sizeof(send_buf));
  fclose(fp);
  return 1;
}

int start_option(){                         // return 0 で成功
  bzero(send_buf,sizeof(send_buf));
  recvbuf();                      // buf = モードを選んでください
  printf("%s\n",recv_buf);
  read(0,send_buf,1000);
  sendbuf();
  if(strcmp(send_buf,"practice\n")==0) return 0;   // practice で break;
  recvbuf();
  if(strcmp(recv_buf,"追加するファイル名を入力してください")==0){ // read
    printf("%s\n",recv_buf);
    read(0,send_buf,1000);
    sendbuf();
    recvbuf();
    if(send_file_data(send_buf)){
      //      recvbuf();
      return 1;
    }
  }
  return 1;
}

int main(int argc, char *argv[]){
  //int fd;
  struct hostent *host;
  struct sockaddr_in sa;
  int pnum=0;
  int i;
  FILE *fp;

  if(argc==2) pnum = atoi(argv[1]);
  printflag == atoi(argv[2]);

  if((host = gethostbyname("localhost")) == NULL){
    printf("error: gethostbyname\n");
    return 1;
  }

  if((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    printf("error: socket");
    return 1;
  }

  sa.sin_family = host->h_addrtype;
  bzero((char *)&sa.sin_addr, 12);
  memcpy((char *)&sa.sin_addr, (char *)host->h_addr, host->h_length);

  /*
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(argv[1]);
    sa.sin_port = htons(3000);*/
  
  while(1){
    sa.sin_port = htons(3000+pnum);
    if(connect(fd,(struct sockaddr *)&sa,sizeof(sa))>=0)break;
    pnum++;
    /*
    if(i==30){
      printf("error : connect\n");
      return 0;
    }*/
  }

  dfpc = fopen("debugc.txt","w");

  while(start_option());                                 // practice of read

  /* 練習スタート */
  recvbuf();                                      //ファイル名を入力してください
  while(1){
    printf("%s\n",recv_buf);
    stl = read(0,send_buf,1000);
    sendbuf();                                     //ファイル名を送信

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
  fclose(dfpc);  
  if(close(fd) == -1){
    printf("error: close\n");
    return 1;
  }
  
  return 1;
}
