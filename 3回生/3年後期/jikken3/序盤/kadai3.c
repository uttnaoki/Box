#include <stdio.h>

int main(void){

  char str[100];
  int i;

  i = read(0,str,100);
  write(1,str,i);

  return 0;
}
