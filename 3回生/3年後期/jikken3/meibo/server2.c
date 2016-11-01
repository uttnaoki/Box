#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LEN 1024
#define MAX_STR_LEN 69
#define MAX_PROFILES 10000
#define MAX_ID_LEN 31
#define MAX_BIRTH_LEN 10

int profile_data_nitems = 0;
int back = 0;
int ditems;
int mark = 0;
int flag = 0;
char send_buf[MAX_LINE_LEN + 1];
char recv_buf[MAX_LINE_LEN + 1];
int send_flag;

int sockfd;
int new_sockfd;

int test=0;

struct date{
  int y;
  int m;
  int d;
};

struct profile{
  int          id;
  char         name[MAX_STR_LEN+1];
  struct date  birthday;
  char         home[MAX_STR_LEN+1];
  char         *comment;
};

struct profile profile_data_store[MAX_PROFILES];

/*** 文字列操作関数 ***/
void parse_line(char *line);

void check(int i);

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

int subst(char *str, char c1, char c2)
{
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

int split(char *str, char *ret[], char sep, int max)
{
  int cnt = 0;
	
  ret[cnt++] = str;
	
  while(*str && cnt < max){
    if(*str == sep){
      *str = '\0';
      ret[cnt++] = str + 1;
    }
    str++;
  }
	
  return cnt;
}

struct date *new_date(struct date *d, char *str)
{
  char *ptr[3];
	
  if(split(str, ptr, '-', 3) != 3)
    return NULL;
	
  d->y = atoi(ptr[0]);
  d->m = atoi(ptr[1]);
  d->d = atoi(ptr[2]);
	
  return d;
}

struct profile *new_profile(struct profile *p, char *csv)
{
  char *ptr[5];
	
  if(split(csv, ptr, ',', 5) != 5)
    return NULL;
		
  /* ID: id */
  p->id = atoi(ptr[0]);
	
  /* 学校名: name */
  strncpy(p->name, ptr[1], MAX_STR_LEN);
  p->name[MAX_STR_LEN] = '\0';
	
  /* 設立年月日: birthday */
  if(new_date(&p->birthday, ptr[2]) == NULL)
    return NULL;
	
  /* 所在地: home */
  strncpy(p->home, ptr[3], MAX_STR_LEN);
  p->home[MAX_STR_LEN] = '\0';
	
  /* 備考データ: comment */
  p->comment = (char *)malloc(sizeof(char) * (strlen(ptr[4])+1));
  strcpy(p->comment, ptr[4]);
	
  flag = 1;
  return p;
}
int get_line(FILE *fp, char *line)
{
  if(fgets(line, MAX_LINE_LEN + 1, fp) == NULL)

    return 0;
		
  subst(line, '\n', '\0');
	
  return 1;
}

/*** exec_command switch ***/

void cmd_quit(char *param)
{
  if(flag==0){
    sprintf(send_buf,"exit");
    exit(0);
  }
  if(*param == 'f') exit(0);
  else {
    sprintf(send_buf,"入力されたデータが書き込まれていません\n終了する場合は%%Q f を入力してください\n");
  }
}

void cmd_check()
{
  //  fprintf(stderr, "%d lines\n",profile_data_nitems);
  sprintf(send_buf,"%d lines\n",profile_data_nitems);
}

void print_profile(int i)
{
  char buf[1024];

  bzero(send_buf,sizeof(send_buf));

  sprintf(buf,"Id       %d\n", profile_data_store[i].id);
  strcat(send_buf,buf);
  sprintf(buf,"Name     %s\n", profile_data_store[i].name);
  strcat(send_buf,buf);
  sprintf(buf,"Birthday %04d-%02d-%02d\n", profile_data_store[i].birthday.y, profile_data_store[i].birthday.m, profile_data_store[i].birthday.d);
  strcat(send_buf,buf);
  sprintf(buf,"Home     %s\n", profile_data_store[i].home);
  strcat(send_buf,buf);
  sprintf(buf,"Comment  %s\n", profile_data_store[i].comment);
  strcat(send_buf,buf);

  // strcat(send_buf,buf2);
  //printf("%s\n",send_buf);

  sprintf(send_buf,"%s\n",send_buf);

  sendrecv();

  //printf(recv_buf);
}

void cmd_print(int nitems)
{
  int i,fin = profile_data_nitems;

  bzero(send_buf,sizeof(send_buf));
  sprintf(send_buf,"xxxxxx");

  sendrecv();

  if(strcmp(recv_buf,"recvx")!=0){
    printf("error : recvx\n");
    exit(0);
  }

  if(nitems == 0){
    for(i=0;i<fin;i++){
      print_profile(i);
    }
  }else if(0 < nitems){
    for(i=0;i<nitems;i++){
      print_profile(i);
    }
  }else if(nitems < 0){
    for(i=fin-1;fin+nitems <= i;i--){
      print_profile(i);
    }
  }

  bzero(send_buf,sizeof(send_buf));
  sprintf(send_buf,"receive loop end");
  //  sendbuf();
}

void cmd_read(char *filename)
{
  char line[MAX_LINE_LEN + 1];
  int a,b;
  FILE *fp;

  a=profile_data_nitems;
  //  filename[strlen(filename)-1] = '\0';
  fp = fopen(filename, "r");
  
  if(fp == NULL) {
    fprintf(stderr,"ファイルがありません\n");
    return;
  }
  
  while (get_line(fp, line)){
    parse_line(line);
  }
  b=profile_data_nitems;
  fclose(fp);
  
  ditems = b - a;
  back = 1;
}

void fprintf_profile_csv(int i,FILE *fp)
{
  fprintf(fp,"%d,", profile_data_store[i].id);
  fprintf(fp,"%s,", profile_data_store[i].name);
  fprintf(fp,"%04d-%02d-%02d,", profile_data_store[i].birthday.y
	  , profile_data_store[i].birthday.m, profile_data_store[i].birthday.d);
  fprintf(fp,"%s,", profile_data_store[i].home);
  fprintf(fp,"%s\n", profile_data_store[i].comment);
}

void cmd_write(char *filename)
{
  int i;
  FILE *fp;
  char *file = "writefile.csv";

  if(*filename == 0) fp = fopen(file,"w");
  else fp = fopen(filename, "w");

  for (i = 0; i < profile_data_nitems; i++){
    fprintf_profile_csv(i,fp);
  }

  fclose(fp);

  flag = 0;
}

char *date_to_string(char buf[], struct date *date)
{
  sprintf(buf, "%04d-%02d-%02d", date->y, date->m, date->d);
  return buf;
}

void cmd_find(char *word)
{
  int i;
  struct profile *p;
  char id[MAX_ID_LEN+1];
  char date[MAX_BIRTH_LEN+1];

  bzero(send_buf,sizeof(send_buf));
  sprintf(send_buf,"xxxxxx");

  sendrecv();

  if(strcmp(recv_buf,"recvx")!=0){    //レシーブに入れているかのcheck
    printf("error : recvx\n");
    exit(0);
  }
  

  for(i=0;i<profile_data_nitems;i++){
    p = &profile_data_store[i];
    sprintf(id, "%d", p->id);    

    if(strcmp(id,word) == 0 ||
       strcmp(p->name, word) == 0 ||
       strcmp(date_to_string(date, &(p->birthday)), word) == 0 ||
       strcmp(p->home, word) == 0 ||
       strcmp(p->comment, word) == 0){
      //      printf("%d\n",i+1);
      print_profile(i);
      //      printf("\n");
    }
  }

  bzero(send_buf,sizeof(send_buf));
  sprintf(send_buf,"receive loop end");
}

void swap(struct profile *a, struct profile *b)
{
  struct profile tmp;

  tmp = *a;
  *a = *b;
  *b = tmp;
}

int compare_date(struct date *d1, struct date *d2)
{
  if (d1->y != d2->y) return d1->y - d2->y;
  if (d1->m != d2->m) return d1->m - d2->m;
  return d1->d - d2->d;
}

int profile_compare(struct profile *p1, struct profile *p2, int column)
{
  switch (column){
  case 1:
    return p1->id - p2->id; break;
  case 2:
    return strcmp(p1->name,p2->name); break; /*name*/
  case 3:
    return compare_date(&(p1->birthday),&(p2->birthday)); break; /*birthday*/
  case 4:
    return strcmp(p1->home,p2->home); break; /*home*/
  case 5:
    return strcmp(p1->comment,p2->comment); break; /*comment*/
  }
  return 0;
}

void cmd_sort(int param)
{
  int i, j;
  struct profile *p;

  for (i = 0; i < profile_data_nitems - 1; i++) {
    for (j = 0; j < profile_data_nitems - 1; j++) {
      p = &profile_data_store[j];

      if (profile_compare(p, p+1, param) > 0)
        swap(p, p+1);
    }
  }
  back = 0;
}

void ndelete(int nitems)
{
  int i;
  for(i=0;i<nitems;i++){
    free(profile_data_store[profile_data_nitems-1].comment);
    profile_data_nitems--;
  }
}

void cmd_delete(int param)
{
  int i;
  FILE *fp;
  
  fp = fopen("backup.txt","w");
  mark = 0;

  if(param == 0){
    fprintf_profile_csv(profile_data_nitems-1,fp);
    ndelete(1);
  }
  
  else if(param > 0 && param < profile_data_nitems + 1){
    for(i=0;i<param;i++)
      fprintf_profile_csv(profile_data_nitems-param+i,fp);
    ndelete(param);
  }

  else if(param < 0 && -profile_data_nitems - 1 < param){
    param = -param;
    fprintf_profile_csv(param-1,fp);
    for(i=0;i<(profile_data_nitems - param);i++){
      swap(&profile_data_store[param-1+i]
	   ,&profile_data_store[param+i]);
    }
    ndelete(1);
    mark = param;
  } else {
    fprintf(stderr,"保存件数は %d 件です\n正しい引数を入力してください\n"
      ,profile_data_nitems);
    sprintf(send_buf,"保存件数は %d 件です\n正しい引数を入力してください\n"
      ,profile_data_nitems);
  return;
}
  
fclose(fp);
back=2;
}

void cmd_add(int param)
{
  int i;
  char line[MAX_LINE_LEN+1];  
  struct profile *p;
  mark = -param;

  bzero(send_buf,sizeof(send_buf));
  sprintf(send_buf,"request");
  
  sendrecv();
  printf(recv_buf);

  subst(recv_buf,'\n','\0');
  parse_line(recv_buf);

  p = &profile_data_store[profile_data_nitems-1];

  for(i=0;i<(profile_data_nitems - param);i++){
    swap(p-i-1,p-i);
  }
  
  back = 3;
  sprintf(send_buf,"ok\n");
}

void cmd_back()
{	
  int i;
  struct profile *p;
	
  switch(back){
	
  case 0:
    printf("you can't back\n");	break;
	
  case 1:
    ndelete(ditems);	break;
	
  case 2:
    cmd_read("backup.txt");
    p = &profile_data_store[profile_data_nitems-1];
    if(mark != 0){
      for(i=0;i<profile_data_nitems-mark;i++)
	swap(p-1-i,p-i);
    }
    break;
	
  case 3:
    cmd_delete(mark);	break;
  }
	
  mark=0;
  back=0;
}

/***** exec_command switch end *****/

void exec_command(char cmd, char *param)
{
  switch (cmd){
  case 'Q': cmd_quit(param);  			break;
  case 'C': cmd_check();	        	break;
  case 'P': cmd_print(atoi(param));     	break;
  case 'R': cmd_read(param);                    break;
  case 'W': cmd_write(param);                   break;
  case 'F': cmd_find(param);                    break;
  case 'S': cmd_sort(atoi(param));              break;
  case 'D': cmd_delete(atoi(param));            break;
  case 'A': cmd_add(atoi(param));               break;
  case 'B': cmd_back();		        	break;
  default:
    fprintf(stderr, "Invalid command %c: ignored.\n", cmd);
    break;
  }
}

void parse_line(char *line)
{
  if(line[0] == '%'){
    exec_command(line[1], &line[3]);
  } else if (new_profile(&profile_data_store[profile_data_nitems],line)!=NULL){
    profile_data_nitems++;
    back = 1;
    ditems = 1;
  } else { 
    fprintf(stderr,"入力形式が違います.\n"); 
    sprintf(send_buf,"入力形式が違います.\n");
  }
}

/////////////////////////////////////////////////////////////////

void check(int i){
  printf("check%d\n",i);
}

int main(){
  int writer_len;
  struct sockaddr_in reader_addr; 
  struct sockaddr_in writer_addr;
  char recv_msg[1024];
  char send_msg[1024];

  /* ソケットの生成 */

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("reader: socket");
    exit(1);
  }



  /* 通信ポート・アドレスの設定 */

  bzero((char *) &reader_addr, sizeof(reader_addr));
  reader_addr.sin_family = AF_INET;
  reader_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  reader_addr.sin_port = htons(3000);

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

  while(1){
    bzero(recv_msg,sizeof(recv_msg));
    bzero(send_msg,sizeof(send_msg));
    bzero(send_buf,sizeof(send_buf));
    sprintf(send_buf,"ok\n");
    if(recv(new_sockfd, recv_msg, 1000, 0) == -1){
      printf("error: recv\n");
      return 1;
    }
    //if(strcmp(recv_msg,"q\n")==0) break;      //コメントを外せばqで終了
    else{
      printf("receive %s",recv_msg);
      subst(recv_msg,'\n','\0');
      parse_line(recv_msg);
    }
    sprintf(send_msg,send_buf);


    if(send(new_sockfd, send_msg, strlen(send_msg), 0) == -1){
      printf("error : send\n");
      return 1;
    }
  }

  close(new_sockfd);
  close(sockfd);  /* ソケットを閉鎖 */
}
