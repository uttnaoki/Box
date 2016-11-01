#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

main(){
  int sockfd;
  int new_sockfd;
  int writer_len;
  struct sockaddr_in reader_addr; 
  struct sockaddr_in writer_addr;
  char rvbuf[1000];
  char snbuf[1000];
  int i;

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

  if(recv(new_sockfd, rvbuf, 1000, 0) == -1){
    printf("error: recv\n");
    return 1;
  }

  bzero(snbuf,sizeof(snbuf));
  for(i=0;rvbuf[i] != '\n';i++) snbuf[i] = (char)toupper(rvbuf[i]);
  sprintf(snbuf,"%s\n%d\n",snbuf,i);

  if(send(new_sockfd, snbuf, strlen(snbuf), 0) == -1){
    printf("error : send\n");
    return 1;
  }

  close(sockfd);  /* ソケットを閉鎖 */
}


/*
void simpe_server(int sockfd) {
  char buf[1];
  int buf_len;

  while((buf_len = read(new_sockfd, buf, 1)) > 0) {
    write(1, buf, buf_len);
  }
  close(new_sockfd);   ソケットを閉鎖 
} 

*/
