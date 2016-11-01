#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
int main(int argc,char *argv[]){
  char str[100];
  int fd;
  int i;

  if((fd = open(argv[1],0))<0){
    printf("text.txtがありません\n");
    return(1);
  }
  i = read(fd,str,100);
  write(1,str,i);

  close(fd);
  return 0;

}
