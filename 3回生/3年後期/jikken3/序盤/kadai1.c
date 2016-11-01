#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int main(int argc,char *argv[]){
  char str[100];
  int fd;
  int i;

  if((fd = open(argv[1], O_WRONLY|O_CREAT|O_TRUNC))<0){
    printf("error\n");
    return(1);
  }

  i = read(0,str,100);

  write(fd,str,i);

  close(fd);

  return 0;
}
