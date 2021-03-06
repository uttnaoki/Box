#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>

#define DATA_MAX 1024
#define STR_LEN 50

int data_count=0;

char send_buf[STR_LEN + 1];
char recv_buf[STR_LEN + 1];
int sockfd;
int new_sockfd;

void sendbuf(){
  if(send(new_sockfd, send_buf, strlen(send_buf),0) == -1){
    printf("error : send\n");
    exit(0);
  }
}
void recvbuf(){
  bzero(recv_buf,sizeof(recv_buf));
  if(recv(new_sockfd,recv_buf,1000,0) == -1){
    printf("error : recv\n");
    exit(0);
  }
}

void sendrecv(){
  sendbuf();
  recvbuf();
}

struct tango{
  char japanese[STR_LEN];
  char english[STR_LEN];
};

struct tango tango[DATA_MAX];

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

void dent(char *str){
  subst(str,'\n','\0');
}

void split(char *line,int i){
  int cnt=0;
	
  for(cnt=0;*line != ',';cnt++){
    tango[i].japanese[cnt] = *line;
    line++;
  }
  line++;
  for(cnt=0;*line != '\n';cnt++){
    tango[i].english[cnt] = *line;
    line++;
  }
}

int read_data(char filename[]){
  FILE *fp;
  char line[STR_LEN];
  char str[20];
	
  sprintf(str,"./read/");
  strcat(str,filename);
  if((fp = fopen(str,"r"))==NULL){
    fprintf(stderr,"%s\n","error:can't read file.");
    return 0;
  }	
  for(data_count=0;(fgets(line, DATA_MAX + 1, fp) != NULL);data_count++)
    split(line,data_count);

  return 1;
}

void practice(){
  int i;
  int count=0;
  //char str[STR_LEN];

  bzero(send_buf,sizeof(send_buf));
  sprintf(send_buf,"start\n");
  sendbuf();
  recvbuf();                      //buf = "ok\n"
  printf(recv_buf);
  for(i=data_count;i>0;i--){
    //bzero(str,sizeof(str));
    sprintf(send_buf,"%s(%d/%d)\n",
	    tango[data_count-i].japanese,
	    data_count-i+1, data_count);
    sendbuf();
    recvbuf();                   // ここで解答が送られる
    //fgets(str,STR_LEN+1,stdin);
    //subst(str,'\n','\0');
    printf(recv_buf);
    dent(recv_buf);
    if(strcmp(recv_buf,tango[data_count-i].english)==0){
      sprintf(send_buf,"correct\n\n");
      count++;
    }else sprintf(send_buf,"incorrect(%s)\n\n",tango[data_count-i].english);
    sendbuf();                     // 正誤判定を送信
    recvbuf();                     // buf = "next\n"
    printf(recv_buf);
  }
  sprintf(send_buf,"result : %d / %d\n",count,data_count);
}

int file_name(char *buf, char dirname[], char ext[]){
  DIR *dirp;
  struct dirent *p;
  char str[1024];

  bzero(buf,sizeof(buf));
	
  if((dirp = opendir(dirname)) == NULL){
    sprintf(buf,"Can't open directory %s\n",dirname);
    return 1;
  }
  while((p = readdir(dirp)) != NULL){
    if(strstr(p->d_name,ext)){
      sprintf(str,"%s ",p->d_name);
      strcat(buf,str);
    }
  }
  strcat(buf,"\n");
  if(closedir(dirp) != 0){
    sprintf(buf,"Can't close directory %s\n",dirname);
    return 1;
  }
  return 0;
}

//////////// 以下main部 /////////////////////

void check(int i){
  printf("check%d\n",i);
}

int main(){
  int writer_len;
  struct sockaddr_in reader_addr; 
  struct sockaddr_in writer_addr;
  char recv_msg[1024];
  char send_msg[1024];
  char buf[1024];
  int i=1;
  int pid;
  pid_t ppid,cpid;
  int pnum;

  ppid = getpid();      //親のpidを取得
  
  for(i=0;i<7;i++) pid = fork(); 
  cpid = getpid();
  printf("%d\n",cpid-ppid);

  if(pid < 0){
    printf("error fork %d\n",cpid-ppid);
  }else if(pid == 0){
    pnum = (int)cpid - (int)ppid;
  }
  else pnum = 0;

  /* ソケットの生成 */

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("reader: socket");
    exit(1);
  }

  /* 通信ポート・アドレスの設定 */

  bzero((char *) &reader_addr, sizeof(reader_addr));
  reader_addr.sin_family = AF_INET;
  reader_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  reader_addr.sin_port = htons(3000 + pnum);

  /* ソケットにアドレスを結びつける */

  if (bind(sockfd, (struct sockaddr *)&reader_addr, sizeof(reader_addr)) < 0) {
    perror("reader: bind");
    exit(1);
  }

  /* コネクト要求をいくつまで待つかを設定 */

  if (listen(sockfd, 5) < 0) {
    perror("reader: listen");
    close(sockfd);
    exit(1);
  }

  /* コネクト要求を待つ */

  writer_len = sizeof(struct sockaddr);
  

  if ((new_sockfd = accept(sockfd,(struct sockaddr *)&writer_addr, &writer_len)) < 0) {
    printf("reader: accept\n");
    exit(1);
  }

  close(sockfd);

  while(1){
    sprintf(send_buf,"\n練習ファイル一覧\n");
    if(file_name(buf,"./read",".csv")) exit(1);
    strcat(send_buf,buf);
    sprintf(buf,"\nファイル名を入力してください");
    strcat(send_buf,buf);
    sendbuf();
    recvbuf();
    dent(recv_buf);
    //if(strcmp(recv_msg,"q")==0) break;      //コメントを外せばqで終了
    if(read_data(recv_buf)) break;
  }
  practice();
  sendbuf();
  close(new_sockfd);
}    

 
