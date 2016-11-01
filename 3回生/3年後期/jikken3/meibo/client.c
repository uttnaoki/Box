/* $Id: http-client.c,v 1.6 2013/01/23 06:57:19 68user Exp $ */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <arpa/inet.h>

void check(int x){
  printf("check %d\n",x);
}

int main(int argc, char *argv[]){
  int fd;
  struct hostent *host;
  struct sockaddr_in sa;
  char send_msg[1000];
  char recv_msg[1000];
  int stl;

  if((host = gethostbyname(argv[1])) == NULL){
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

  while(1){
    bzero(send_msg,sizeof(send_msg));
    bzero(recv_msg,sizeof(recv_msg));
    stl = read(0,send_msg,1000);

    if(send(fd, send_msg, stl, 0) == -1){
      printf("error: send\n");
      return 1;
    }

    if(strcmp(send_msg,"q\n")==0)break;             //コメントを外せばqで終了可能
    if(strcmp(send_msg,"%Q f\n")==0)break;

    if(recv(fd,recv_msg,1000,0)==-1){
      printf("error:recv\n");
      return 1;
    }

    if(strcmp(recv_msg,"exit")==0)break;
    if(strcmp(recv_msg,"request")==0){
      printf("データを1件入力してください\n");
      stl = read(0,send_msg,1000);
      if(send(fd,send_msg,stl,0) == -1){
	printf("error: send\n");
	return 1;
      }
      bzero(recv_msg,sizeof(recv_msg));
      if(recv(fd,recv_msg,1000,0)==-1){
	printf("error:recv\n");
	return 1;
      }
    }

    //printf("\ncheck %s",send_msg);
    //printf("check %saaa\n\n",recv_msg);
    if(strcmp(recv_msg,"xxxxxx")==0){                //ここに入ればレシーブループ
      bzero(send_msg,sizeof(send_msg));
      sprintf(send_msg,"recvx");
      if(send(fd,send_msg,strlen(send_msg),0)==-1){
	printf("error: send\n");
	return 1;
      }

      bzero(recv_msg,sizeof(recv_msg));
      if(recv(fd,recv_msg,1000,0)==-1){
	printf("error:recv\n");
	return 1;
      }

      while(strcmp(recv_msg,"receive loop end")!=0){  //ループ開始
	printf(recv_msg);

	bzero(send_msg,sizeof(send_msg));
	sprintf(send_msg,"receive loop");
	if(send(fd,send_msg,strlen(send_msg),0)==-1){
	  printf("error: send\n");
	  return 1;
	}

	bzero(recv_msg,sizeof(recv_msg));
	if(recv(fd,recv_msg,1000,0)==-1){
	  printf("error:recv\n");
	  return 1;
	}
      }                                              //ここまでレシーブループ
    }else{
      printf(recv_msg);
    }
  }

  if(close(fd) == -1){
    printf("error: close\n");
    return 1;
  }

  return 1;
}
