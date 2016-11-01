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
  int buf_len;
  int stl;
  int i;

  int a=0,b=0,c=0;

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

    if(strcmp(send_msg,"q\n")==0)break;

    if(recv(fd,recv_msg,1000,0)==-1){
      printf("error:recv\n");
      return 1;
    }
    if(strcmp(recv_msg,"xxxxxx")==0){
      printf(recv_msg);
	
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

      bzero(send_msg,sizeof(send_msg));
      sprintf(send_msg,"loop%s\n",recv_msg);
      if(send(fd,send_msg,strlen(send_msg),0)==-1){
	printf("error: send\n");
	return 1;
      }

      for(i=atoi(recv_msg);i>0;i--){
	bzero(recv_msg,sizeof(recv_msg));
	if(recv(fd,recv_msg, 1000, 0)==-1){
	  printf("error:recv\n");
	  return 1;
	}
	printf(recv_msg);
	bzero(send_msg,sizeof(send_msg));
	sprintf(send_msg,"received(%d)\n",i);
	if(send(fd,send_msg,strlen(send_msg),0)==-1){
	  printf("error: send\n");
	  return 1;
	}
      }
      if(recv(fd,recv_msg,10000,0)==-1){
	printf("error:recv\n");
	return 1;
      }
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
