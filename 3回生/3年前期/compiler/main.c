#include <stdio.h>
#include <stdlib.h>	/* free() を使う時に必要 */
#include "define.h"

/* 字句解析関数のプロトタイプ宣言                                       */
/* この関数は,字句を切り出して,TokenSt構造体に入れて,そのポインタを返す */
/* これ以上字句がない時は NULLを返す                                    */
/* なお，構造体を保持するメモリは,内部で mallocしている                 */
TokenSt *nextToken(FILE *fp);
Node *Oparser(FILE *fp);
void program(FILE *fp);

/* 使い方表示関数のプロトタイプ宣言                */
/* このファイルでしか使わないので staticにしている */
static void usage(char *s);


int main(int argc, char *argv[]){
  FILE		*fp;
  //  TokenSt	*token;
  // Node          *node;
  char          data[10][100];

  FILE          *cfp;
  


  /*--< ファイル名を指定しないとエラー >--*/
  if (argc != 2 ) usage(argv[0]);

  if((rfp = fopen("result.s", "w"))==NULL){
    printf("file open error\n");
    exit(1);
  }
  
  /*--< ファイルをオープンする >--*/
  if ((fp = fopen(argv[1],"r")) == NULL){
    fprintf(stderr,"No such file (%s)\n",argv[1]);
    exit(1);
  }

  /*--< 字句を順番に取り出す )--*/
  /*
  while ( (token = nextToken(fp)) != NULL ){
    printf("Type:%02d  String:[%s]\n",token->type, token->string);
    free(token); 
  }
  */
  program(fp);
  printf("check\n");

  /*--< 後始末 >--*/
  fclose(fp);
  fclose(rfp);
  
  ///////////////////////////////
  if((rfp = fopen("result.s", "r"))==NULL){
    printf("No such file\n");
    exit(1);
  }

  if((cfp = fopen("compresult.s", "w"))==NULL){
    printf("file open error\n");
    exit(1);
  }
  
  while(fgets(data, 100, rfp) != NULL){
    if(strcmp(data,"\taddi $sp,$sp,-4\n") == 0){
      fgets(data[1], 100, rfp);
      if(strcmp(data[1], "\tsw $v0,0($sp)\n") == 0){
	fgets(data[2], 100, rfp);
	if(strcmp(data[2], "\tsw $v0, 0($t2)    #変数の値を更新\n") ==0 ||
	   strcmp(data[2], "\tadd $t8, $v0, $zero\n") == 0){
	  fgets(data[3], 100, rfp);
	  if(strcmp(data[3], "\taddi $sp, $sp, 4\n") == 0){
	    fprintf(cfp,data[2]);
	  }else{
	    fprintf(cfp,data);
	    fprintf(cfp,data[1]);
	    fprintf(cfp,data[2]);
	    fprintf(cfp,data[3]);
	  }
	}else if(strcmp(data[2], "\taddi $sp, $sp, 4\n") == 0){
	}else{
	  fprintf(cfp,data);
	  fprintf(cfp,data[1]);
	  fprintf(cfp,data[2]);
	}
      }else{
	fprintf(cfp,data);
	fprintf(cfp,data[1]);
      }
    }else{
      fprintf(cfp,data);
    }
  }

  fclose(cfp);
  fclose(rfp);
  ///////////////////////////////

  return 0;
}

/*--< このプログラムの説明を表示 >--*/
static void usage(char *s){
  fprintf(stderr, "Usage: %s < filename >\n",s);
  exit(1);
}
