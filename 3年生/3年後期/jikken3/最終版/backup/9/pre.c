#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATA_MAX 1024
#define STR_LEN 50

int data_count=0;

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
	
	if((fp = fopen(filename,"r"))==NULL){
	fprintf(stderr,"%s\n","error:can't read file.");
	return EXIT_FAILURE;
	}	
	for(data_count=0;(fgets(line, DATA_MAX + 1, fp) != NULL);data_count++)
	split(line,data_count);

	return 1;
}

void practice(){
  int i;
  char str[STR_LEN];

  printf("start\n");
  for(i=data_count;i>0;i--){
    memset(str,0,sizeof(str));
    printf("%s(%d/%d)\n",
	   tango[data_count-i].japanese,
	   data_count-i+1, data_count);
    fgets(str,STR_LEN+1,stdin);
    subst(str,'\n','\0');
    if(strcmp(str,tango[data_count-i].english)==0)
      printf("correct\n\n");
    else printf("incorrect(%s)\n\n",tango[data_count-i].english);
  }
  printf("end\n");
}

int main()
{
  	char file_name[]="test.csv";
	
	read_data(file_name);
	practice();
	
  	return 0;
}
