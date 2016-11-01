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
int printflag=0;
FILE *dfps;
int accountflag;
char accountname[20];

void sendbuf(){
  if(send(new_sockfd, send_buf, strlen(send_buf),0) == -1){
    printf("error : send\n");
    exit(0);
  }
  if(printflag==1){
    fprintf(dfps,"debugs %s\n",send_buf);
    fprintf(dfps,"================================\n");
  }
  if(printflag==2){
    printf("=============send===================\n");
    printf("%s\n",send_buf);
    printf("=============send===================\n");
  }
}
void recvbuf(){
  bzero(recv_buf,sizeof(recv_buf));
  if(recv(new_sockfd,recv_buf,1000,0) == -1){
    printf("error : recv\n");
    exit(0);
  }
  if(printflag==1){
    fprintf(dfps,"debugr %s\n",recv_buf);
    fprintf(dfps,"********************************\n");
  }
  if(printflag==2){
    printf("************recv*******************\n");
    printf("%s\n",recv_buf);
    printf("************recv*******************\n");
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

  fclose(fp);

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
  printf("%s\n",recv_buf);
  for(i=data_count;i>0;i--){
    //bzero(str,sizeof(str));
    sprintf(send_buf,"%s(%d/%d)\n",
	    tango[data_count-i].japanese,
	    data_count-i+1, data_count);
    sendbuf();
    recvbuf();                   // ここで解答が送られる
    //fgets(str,STR_LEN+1,stdin);
    //subst(str,'\n','\0');
    printf("%s\n",recv_buf);
    dent(recv_buf);
    if(strcmp(recv_buf,tango[data_count-i].english)==0){
      sprintf(send_buf,"correct\n\n");
      count++;
    }else sprintf(send_buf,"incorrect(%s)\n\n",tango[data_count-i].english);
    sendbuf();                     // 正誤判定を送信
    recvbuf();                     // buf = "next\n"
    printf("%s\n",recv_buf);
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

int read_file(){
  FILE *fp;
  char file[50];

  sprintf(file,"./read/");
  sprintf(send_buf,"追加するファイル名を入力してください");
  sendbuf();
  recvbuf();
  if(strcmp(recv_buf,"can't open file")==0) return 0;
  dent(recv_buf);
  strcat(file,recv_buf);
  fp = fopen(file,"w");
  sprintf(send_buf,"ok");
  sendbuf();
  // データ受取スタート
  recvbuf();
  while(strcmp(recv_buf,"EOF")!=0){
    fprintf(fp,"%s",recv_buf);
    sendbuf();
    recvbuf();
  }
  fclose(fp);
  return 0;
}

int new_account(){
  int i;
  char buf[100]="./account/";
  sprintf(send_buf,"作成するアカウント名を入力してください");
  sendbuf();
  recvbuf();
  dent(recv_buf);
  strcat(buf,recv_buf);
  i=mkdir(buf,0777);
  if(i==0)sprintf(send_buf,"アカウントを作成しました");
  else sprintf(send_buf,"このアカウントは使えません");
  sendbuf();
  recvbuf();
  return i;
}

int log_in(){
  DIR *dirp;
  struct dirent *p;
  int find=0;

  sprintf(send_buf,"アカウント名を入力してください");
  sendbuf();
  recvbuf();
  dent(recv_buf);
  dirp = opendir("account");
  while((p = readdir(dirp)) != NULL){
    if(strcmp(p->d_name,recv_buf)==0) find=1;
  }
  if(find==0) sprintf(send_buf,"アカウント名が間違っています");
  else{
    sprintf(send_buf,"ログイン：%s",recv_buf);
    accountflag=1;
    strcpy(accountname,recv_buf);
  }
  sendbuf();
  recvbuf();
  return 0;
}

int log_out(){
  accountflag=0;
  bzero(accountname,sizeof(accountname));
  sprintf(send_buf,"ログアウトしました");
  sendbuf();
  recvbuf();
  return 0;
}

//////////// 以下main部 /////////////////////

void check(int i){
  printf("check%d\n",i);
}

int main(int argc,char *argv[]){
  int writer_len;
  struct sockaddr_in reader_addr; 
  struct sockaddr_in writer_addr;
  char recv_msg[1024];
  char send_msg[1024];
  char buf[1024];
  int i=0;
  int pid;
  pid_t ppid,cpid;
  int pnum;

  ppid = getpid();      //親のpidを取得
  
  for(i=0;i<0;i++) pid = fork(); 
  cpid = getpid();
  printf("%d\n",cpid-ppid);

  if(pid < 0) printf("error fork %d\n",cpid-ppid);
  else if(pid == 0){
    pnum = (int)cpid - (int)ppid;
  }
  else pnum = 0;

if(argc==3) printflag = atoi(argv[2]);

  /* ソケットの生成 */

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("reader: socket");
    exit(1);
  }

  /* 通信ポート・アドレスの設定 */

  bzero((char *) &reader_addr, sizeof(reader_addr));
  reader_addr.sin_family = AF_INET;
  reader_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  /* ソケットにアドレスを結びつける */
  /*
  while(1){
    reader_addr.sin_port = htons(4000 + i);
    if(bind(sockfd,(struct sockaddr *)&reader_addr,sizeof(reader_addr))==0)break;
    i++;
    if(i==30){
      printf("error : bind\n");
      return 0;
    }
    }*/

  while(1){
    reader_addr.sin_port = htons(3000 + pnum);
    if(bind(sockfd,(struct sockaddr *)&reader_addr,sizeof(reader_addr))==0)break;
    pnum++;
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

  dfps = fopen("debugs.txt","w");
  accountflag=0;
  bzero(accountname,sizeof(accountname));

  while(1){
    while(1){
      if(accountflag==0){
	sprintf(send_buf,"\nモードを選んでください\n");
	strcat(send_buf,"practice\nread\nnew account\nlog in\n");
	sendbuf();
	recvbuf();
	if(strcmp(recv_buf,"practice\n")==0){
	  //sprintf(send_buf,"practice");
	  //sendbuf();
	  break;
	}else if(strcmp(recv_buf,"read\n")==0){
	  read_file();
	}else if(strcmp(recv_buf,"new account\n")==0){
	  new_account();
	}else if(strcmp(recv_buf,"log in\n")==0){
	  log_in();
	}
      }else{
	sprintf(send_buf,"\nモードを選んでください\n");
	strcat(send_buf,"practice\nread\nlog out\n");
	sendbuf();
	recvbuf();
	if(strcmp(recv_buf,"practice\n")==0) break;
	else if(strcmp(recv_buf,"read\n")==0) read_file();
	else if(strcmp(recv_buf,"log out\n")==0) log_out();
      }
    }
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
  fclose(dfps);
}    

 
