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
char send_buf[1024];
char recv_buf[1024];
int stl;
int i=0;
int printflag=0;
int accountflag=0;
FILE *dfpc;
int rflag=0;

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

  sendbuf();
  recvbuf();
  
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
  if(strcmp(send_buf,"quit\n")==0) return -1;

  else if(accountflag==0){
    if((strcmp(send_buf,"read\n")!=0)&&
       (strcmp(send_buf,"new account\n")!=0)&&
       (strcmp(send_buf,"log in\n")!=0)) return 1;
  }else if(accountflag==1){
    if(strcmp(send_buf,"review\n")==0){
      rflag=1;
      return 0;
    }else if((strcmp(send_buf,"read\n")!=0)&&
       (strcmp(send_buf,"log out\n")!=0)) return 1;
  }
  recvbuf();
  if(strcmp(recv_buf,"追加するファイル名を入力してください")==0){ // read
    printf("%s\n",recv_buf);
    read(0,send_buf,1000);
    if(send_file_data(send_buf)){    // s s
      //      recvbuf();
      return 1;
    }else{
      sprintf(send_buf,"can't open file");
      sendbuf();
      return 1;
    }
  }else if(strcmp(recv_buf,"作成するアカウント名を入力してください")==0){
    printf("%s\n",recv_buf);
    read(0,send_buf,1000);
    sendbuf();
    recvbuf();
    printf("%s\n",recv_buf);
    bzero(send_buf,sizeof(send_buf));
    sprintf(send_buf,"na end");
    sendbuf();
    return 1;
  }else if(strcmp(recv_buf,"アカウント名を入力してください")==0){
    printf("%s\n",recv_buf);
    read(0,send_buf,1000);
    sendbuf();
    recvbuf();
    if(strstr(recv_buf,"ログイン")) accountflag=1;
    printf("%s\n",recv_buf);
    sprintf(send_buf,"login");
    sendbuf();
    return 1;
  }else if(strcmp(recv_buf,"ログアウトしました")==0){
    printf("%s\n",recv_buf);
    accountflag=0;
    sprintf(send_buf,"logout");
    sendbuf();
    return 1;
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
  int eflag;

  if(argc==2) pnum = atoi(argv[1]);
  //printflag = atoi(argv[2]);

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
  sa.sin_port = htons(pnum);

  if(connect(fd,(struct sockaddr *)&sa, sizeof(sa))==-1){
    printf("error:connect\n");
    return 1;
  }

  dfpc = fopen("debugc.txt","w");

  while(1){
    while((eflag=start_option())==1);
    if(eflag==-1)break;
    /* 練習スタート */
      recvbuf();                                      
    if(rflag==0){
      while(1){
	printf("%s\n",recv_buf);
	stl = read(0,send_buf,1000);
	sendbuf();                                     
	recvbuf();                                     // buf = "start\n"
	if(strcmp(recv_buf,"start\n")==0) break;
      }
    }
    printf("\n\n%s\n",recv_buf);
    sprintf(send_buf,"recvstart\n");
    sendbuf();
    recvbuf();                                     // 1問目
    while(strncmp(recv_buf,"result",6)){
      printf("%s\n",recv_buf);
      bzero(send_buf,sizeof(send_buf));
      stl=read(0,send_buf,1000);
      sendbuf();
      recvbuf();
      printf("%s\n",recv_buf);
      sprintf(send_buf,"next\n");
      sendbuf();
      recvbuf();
    }                                             // recvでループを抜ける
    printf("%s\n",recv_buf);
    sprintf(send_buf,"practice end");
    sendbuf();
    rflag=0;
  }
  fclose(dfpc);  
  if(close(fd) == -1){
    printf("error: close\n");
    return 1;
  }
  
  return 1;
}
