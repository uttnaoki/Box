#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include "define.h"

Node *Oparser(FILE *fp);

void hensusengen(FILE *fp);
void sengenbun(FILE *fp);
void soejiretsu(FILE *fp, int x);
void kansugun(FILE *fp);
void kansu(FILE *fp);
void bunshu(FILE *fp);
void whilebun(FILE *fp);
void forbun(FILE *fp);
void ifbun(FILE *fp);
void dainyubun(FILE *fp);
void kansubun(FILE *fp);
void jokenshiki(FILE *fp);
void dainyushiki(FILE *fp, int i);
void jokenshori(int lgc);
void jokenshori2(int lgc);

Vartable id[TOKENMAX];
Functable fid[TOKENMAX];
int vi=1,fi=0;

int ifflag=1;
int lgcount=1;      //ループ生成回数

Looptable looptable[100];

// プログラムの解析
void program(FILE *fp){
  TokenSt *token;
  int n;
  int x,y,z;

  freeflag=1;

  strcpy(id[0].name, "result");
  id[0].addr = 0x10004000;
  id[0].next = 0x10004004;

  printf("プログラム開始\n");
  
  fprintf(rfp,
	  "\tINITIAL_GP = 0x10008000\n"
	  "\tINITIAL_SP = 0x7ffffffc\n"
	  "\tframesize = 32\n"
	  "\tra_loc = 28\n"
	  "\tfp_loc = 24\n"
	  "\t# system call service number\n"
	  "\tstop_service = 99\n\n");

  // fprintf(rfp,"\t.text   0x00001000\n");
  fprintf(rfp,	  "init:\n"
	  "\t# initialize $gp (global pointer) and $sp (stack pointer)\n"
	  "\tla      $gp, INITIAL_GP         # $gp <- 0x10008000 (INITIAL_GP)\n"
	  "\tla      $sp, INITIAL_SP         # $sp <- 0x7ffffffc (INITIAL_SP)\n"
	  "\tjal     main                    # jump to `main'\n"
	  "\tnop                             #   (delay slot)\n"
	  "\tli      $v0, stop_service       # $v0 <- 99 (stop_service)\n"
	  "\tsyscall                         # halt\n"
	  "\tnop\n"
	  "\t# not reach here\n" );

  fprintf(rfp,
	  "stop:                                   #if syscall return\n"
	  "\tj stop                          #infinite loop...\n"
	  "\tnop\n\n\n");

  token = nextToken(fp);
  if(token->type == DEFINE){
    ungetToken();
    hensusengen(fp);
    token = nextToken(fp);
  }
  
  if(token->type != IDENT) exit(1);
  ungetToken();
  kansugun(fp);

  fprintf(rfp,"\t.data 0x10004000\n");
  for(n=0;vi-n;n++) {
    if(id[n].subcount==0)fprintf(rfp,"%s: .word 0\n",id[n].name);
    else{
      z = (id[n].addramount[0] * id[n].subamount[0]) / 4;
      x = z / 8;
      y = z % 8;
      if(x){
	fprintf(rfp,"%s: .word 0, 0, 0, 0, 0, 0, 0, 0\n",id[n].name);
	x--;
	while(x){
	  fprintf(rfp," .word 0, 0, 0, 0, 0, 0, 0, 0\n");
	  x--;
	}
	if(y) fprintf(rfp," .word 0");
	y--;
      }else{
	fprintf(rfp,"%s: .word",id[n].name);
	if(y){
	  fprintf(rfp," 0");
	  y--;
	}
      }
      while(y){
	fprintf(rfp,", 0");
	y--;
      }
      fprintf(rfp,"\n");
    }    
  }
  printf("プログラム終了\n");
}

// 変数宣言部の解析
void hensusengen(FILE *fp){
  TokenSt *token;
  printf("変数宣言部開始\n");
  token = nextToken(fp);
  while(token->type == DEFINE){
    sengenbun(fp);
    token = nextToken(fp);
  }
  ungetToken();
  printf("変数宣言部終了\n");
}
   
// 宣言文の解析
void sengenbun(FILE *fp){
  TokenSt *token;
  int subcount=0;
  int x;

  printf("宣言文開始\n");
  token = nextToken(fp);
 
  if(token->type != IDENT) exit(1);

  strcpy(id[vi].name, token->string);     // 変数名を Vartable に格納
  id[vi].addr = id[vi-1].next;
  id[vi].next = id[vi].addr + 4;

  while(1){
    token = nextToken(fp);
    if(token->type != KAKKO2L) break;
    token = nextToken(fp);
    if(token->type != INTEGER) exit(1);
    id[vi].subamount[subcount] = atoi(token->string);
    token = nextToken(fp);
    if(token->type != KAKKO2R) exit(1);
    subcount++;
  }
  if(subcount){       // 配列のVartabel処理
    id[vi].subcount = subcount;
    x=subcount-1;
    id[vi].addramount[x]=4;
    for(x=subcount-1;x>=0;x--){
      id[vi].addramount[x-1] = id[vi].addramount[x] * id[vi].subamount[x];
    }
    id[vi].next = id[vi].addr + id[vi].addramount[0] * id[vi].subamount[0];
  }

  if(token->type != SEMICOLON) exit(1);
  vi++;
  printf("宣言文終了\n");
}


// 添字列の解析
void soejiretsu(FILE *fp, int x){
  TokenSt *token;
  int n=id[x].subcount;
  int i=0;

  printf("添字列開始\n");
  fprintf(rfp,"\tadd $t5, $zero, $zero        #添字解析\n");
  while(n){
    token = nextToken(fp);
    if(token->type != KAKKO2L) break;
    argflag=1;
    printTree(Oparser(fp));
    argflag=0;
    fprintf(rfp,"\taddi $sp, $sp, 4\n");
    fprintf(rfp,"\tli $t4, %d\n",id[x].addramount[i]);
    fprintf(rfp,"\tmult $v0, $t4\n");
    fprintf(rfp,"\tmflo $t3\n");
    fprintf(rfp,"\tadd $t5, $t5, $t3\n");

    token = nextToken(fp);
    if(token->type != KAKKO2R) exit(1);
    i++;n--;
  }
  fprintf(rfp,"\tadd $t2, $t2, $t5\n");
  printf("添字列終了\n");
}

// 関数群の解析
void kansugun(FILE *fp){
  printf("関数群開始\n");

  while(1){
    if(nextToken(fp) == NULL) break;
    
    ungetToken();
    if(nextToken(fp)->type == IDENT){
      ungetToken();
      kansu(fp);
    }else {
      exit(1);
    }
  }
  printf("関数群終了\n");
}
  
// 関数の解析
void kansu(FILE *fp){
  TokenSt *token;
  printf("関数開始\n");
  token = nextToken(fp);
  if(token->type != IDENT) exit(1);

  fprintf(rfp,"%s:\n", token->string);
  fprintf(rfp,
	  "\taddi    $sp, $sp, -framesize    # allocate new stack frame\n"
	  "\tsw      $ra, ra_loc($sp)        # save $ra to stack frame\n"
	  "\tsw      $fp, fp_loc($sp)        # save $fp to stack frame\n"
	  "\tadd     $fp, $sp, $zero             # $fp <- $sp\n\n");

  strcpy(fid[fi++].name, token->string);       //関数名を Functable に格納
  token = nextToken(fp);
  if(token->type != KAKKO1L) exit(1);
  token = nextToken(fp);
  
  while(1){
    if(token->type != IDENT){
      break;
    }
    token=nextToken(fp);
    if(token->type == KAKKO2L){
      token = nextToken(fp);
      if(token->type != KAKKO2R) exit(1);
      token = nextToken(fp);
    }
    if(token->type != COMMA) break;
    token = nextToken(fp);
  }
  if(token->type != KAKKO1R) exit(1);
  token = nextToken(fp);
  if(token->type != KAKKO3L) exit(1);  // {
  token = nextToken(fp);
  if(token->type == DEFINE){
    ungetToken();
    hensusengen(fp); // 変数宣言部
  }
  ungetToken();
  bunshu(fp);
  token = nextToken(fp);
  if(token->type != KAKKO3R) exit(1);// }

  fprintf(rfp,
	  "\tadd $sp, $fp, $zero\n"
	  "\tnop\n"
	  "\tlw $fp, fp_loc($sp)\n"
	  "\tlw $ra, ra_loc($sp)\n"
	  "\tnop\n"
	  "\taddi $sp, $sp, framesize\n");

  fprintf(rfp,
	  "\tjr $ra\n"
	  "\tnop\n");
  printf("関数終了\n");
}

// 文集合の解析
void bunshu(FILE *fp){
  TokenSt *token;
  int k;
  printf("文集合開始\n");
  while(1){
    token = nextToken(fp);
    if(token->type == WHILE){
      whilebun(fp);                 //while文
    }else if (token->type == FOR){
      forbun(fp);                   //for文
    }else if(token->type == IF){
      ifbun(fp);                    //if文
    }else if (token->type==IDENT){
      k=getc(fp);
      if(k=='(') {
	ungetToken();
	ungetc((unsigned char)k,fp);
	kansubun(fp);               //関数文
      }else {
	ungetc((unsigned char)k,fp);
	ungetToken();
	dainyubun(fp);              //代入文
      }
    }else if (token->type==BREAK){
      token = nextToken(fp);
      if(token->type != SEMICOLON) exit(1);
      break;
    }else {
      ungetToken();
      break;
    }
  }
  printf("文集合終了\n");
}

// while文の解析
void whilebun(FILE *fp){
  TokenSt *token;
  int lgc;

  lgc=lgcount++;
  
  printf("while文開始\n");

  token = nextToken(fp);
  if(token->type != KAKKO1L) exit(1);

  jokenshiki(fp);                     // $t8(アドレス) <? $t9(数値)

  token = nextToken(fp);
  if(token->type != KAKKO1R) exit(1); //(<条件式>)

  token = nextToken(fp); 
  if(token->type != KAKKO3L) exit(1);

  freeflag=0;
  jokenshori(lgc);
  freeflag=1;

  fprintf(rfp,"LoopS%d:\n",lgc);

  bunshu(fp);

  jokenshori2(lgc);

  fprintf(rfp,"LoopE%d:\n",lgc);

  token = nextToken(fp);
  if(token->type != KAKKO3R) exit(1); //{<文集合>}
  printf("while文終了\n");
}

void jokenshori(int lgc){

  fprintf(rfp,
	  "\tla $t8, %s\n"
	  "\tlw $t8, 0($t8)\n"
	  "\tnop\n", looptable[lgc].string);
  
  printTree(looptable[lgc].node);

  switch(looptable[lgc].flag){
  case 1: // ==
    fprintf(rfp,
	    "\tsub $t3, $t8, $v0\n"
	    "\tbne $t3, $zero, LoopE%d\n"
	    "\tnop\n",lgc);
    break;
  case 2: // <
    fprintf(rfp,
	    "\tslt $t3, $t8, $v0\n"
	    "\tbeq $t3, $zero, LoopE%d\n"
	    "\tnop\n",lgc);
    break;
  case 3: // >
    fprintf(rfp,
	    "\tslt $t3, $v0, $t8\n"
	    "\tbeq $t3, $zero LoopE%d\n"
	    "\tnop\t",lgc);
    break;
  case 4: // !=
    fprintf(rfp,
	    "\tsub $t3, $t8, $v0\n"
	    "\tbeq $t3, $zero, LoopE%d\n"
	    "\tnop\n",lgc);
    break;
  case 5: // <=
    fprintf(rfp,
	    "\tslt $t3, $v0, $t8\n"
	    "\tbne $t3, $zero, LoopE%d\n"
	    "\tnop\n",lgc);
    break;
  case 6: // >=
    fprintf(rfp,
	    "\tslt $t3, $t8, $v0\n"
	    "\tbne $t3, $zero LoopE%d\n"
	    "\tnop\n",lgc);
    break;
  }
}

void jokenshori2(int lgc){

  fprintf(rfp,
	  "\tla $t8, %s\n"
	  "\tlw $t8, 0($t8)\n"
	  "\tnop\n", looptable[lgc].string);
  
  printTree(looptable[lgc].node);

  switch(looptable[lgc].flag){
  case 1: // ==
    fprintf(rfp,
	    "\tsub $t3, $t8, $v0\n"
	    "\tbeq $t3, $zero, LoopS%d\n"
	    "\tnop\n",lgc);
    break;
  case 2: // <
    fprintf(rfp,
	    "\tslt $t3, $t8, $v0\n"
	    "\tbne $t3, $zero, LoopS%d\n"
	    "\tnop\n",lgc);
    break;
  case 3: // >
    fprintf(rfp,
	    "\tslt $t3, $v0, $t8\n"
	    "\tbne $t3, $zero LoopS%d\n"
	    "\tnop\t",lgc);
    break;
  case 4: // !=
    fprintf(rfp,
	    "\tsub $t3, $t8, $v0\n"
	    "\tbne $t3, $zero, LoopS%d\n"
	    "\tnop\n",lgc);
    break;
  case 5: // <=
    fprintf(rfp,
	    "\tslt $t3, $v0, $t8\n"
	    "\tbeq $t3, $zero, LoopS%d\n"
	    "\tnop\n",lgc);
    break;
  case 6: // >=
    fprintf(rfp,
	    "\tslt $t3, $t8, $v0\n"
	    "\tbeq $t3, $zero LoopS%d\n"
	    "\tnop\n",lgc);
    break;
  }
}

// for文の解析
void forbun(FILE *fp){
  TokenSt *token;
  int lgc=lgcount++;;
  char ident[20];
  int x;

  printf("for文開始\n");

  token = nextToken(fp);
  if(token->type != KAKKO1L) exit(1);

  token = nextToken(fp);
  if(token->type != IDENT) exit(1);    //第一式  
  strcpy(ident, token->string);
  fprintf(rfp,"\tla $t2, %s      #変数のアドレスをロード\n",ident);

  for(x=1;x!=vi;x++){
    if(strcmp(id[x].name, ident))break;
    if(x==vi){
      printf("error %s 宣言されていない識別子です\n",ident);
      exit(1);
    }
  }

  token = nextToken(fp);
  if(token->type == KAKKO2L){
    ungetToken();
    soejiretsu(fp, x);
    token = nextToken(fp);
    if(token->type != EQUAL) exit(1);
    token = nextToken(fp);
  }else if(token->type == EQUAL){
    // fprintf(rfp,"\tla $t2, %s\n",ident);
    token = nextToken(fp);
  }else exit(1);

  if(token->type != INTEGER)exit(1);
  fprintf(rfp,
	  "\tli $v0, %s\n"
	  "\tsw $v0, 0($t2)\n",token->string);

  token = nextToken(fp);
  if(token->type != SEMICOLON) exit(1);

  jokenshiki(fp);                     //第二式
  token = nextToken(fp); 
  if(token->type != SEMICOLON) exit(1);
  dainyushiki(fp,lgc);                //第三式
  token = nextToken(fp); 
  if(token->type != KAKKO1R) exit(1); //(<代入文>;<条件式>;<代入式>) <-now

  token = nextToken(fp);
  if(token->type != KAKKO3L) exit(1);
  freeflag=0;
  jokenshori(lgc);                     //$t3に条件式の真偽を判定
  freeflag=1;

  fprintf(rfp,"LoopS%d:\n",lgc);

  bunshu(fp);

  switch(looptable[lgc].forflag){              //第三式の処理
  case 1:
    fprintf(rfp,
	    "\tla $t2, %s\n"
	    "\tlw $v0, 0($t2)\n"
	    "\tnop\n"
	    "\taddi $v0, $v0, 1\n"
	    "\tsw $v0, 0($t2)\n",looptable[lgc].forstring);
    break;
  case 2:
    fprintf(rfp,
	    "\tla $t2, %s\n"
	    "\tlw $v0, 0($t2)\n"
	    "\tnop\n"
	    "\taddi $v0, $v0, -1\n"
	    "\tsw $v0, 0($t2)\n",looptable[lgc].forstring);
    break;
  case 3:
    printTree(looptable[lgc].node);
    fprintf(rfp,
	    "\tla $t2, %s\n"
	    "\tsw $v0, 0($t2)\n",looptable[lgc].forstring);
    break;
  default:
    exit(1);
  }

  jokenshori2(lgc);                   //第二式の条件処理

  token = nextToken(fp);
  if(token->type != KAKKO3R) exit(1); //{<文集合>}

  fprintf(rfp,"LoopE%d:\n",lgc);

  printf("for文終了\n");
}

// if文の解析
void ifbun(FILE *fp){
  TokenSt *token;
  int ifc;
  TokenType ifope;
  ifc=ifflag++;

  printf("if文開始\n");
  fprintf(rfp,"$if%d:\n",ifc);

  token = nextToken(fp);
  if(token->type != KAKKO1L) exit(1);
  //  jokenshiki(fp);

  printTree(Oparser(fp));          // if("x" < 10)
  fprintf(rfp,
	  "\tadd $t8, $v0, $zero\n"
	  "\taddi $sp, $sp, 4\n");

  token = nextToken(fp);           // if(x "<" 10)
  ifope = token->type;
  
  jflag=1;
  printTree(Oparser(fp));           // if(x < "10")
  fprintf(rfp,"\taddi $sp, $sp, 4\n");
  jflag=0;

  switch(ifope){
  case EQUAL2: // ==
    fprintf(rfp,
	    "\tsub $t3, $t8, $v0\n"
	    "\tbne $t3, $zero, $skipif%d\n"
	    "\tnop\n",ifc);
    break;
  case LESS: // <
    fprintf(rfp,
	    "\tslt $t3, $t8, $v0\n"
	    "\tbeq $t3, $zero, $skipif%d\n"
	    "\tnop\n",ifc);
    break;
  case GREAT: // >
    fprintf(rfp,
	    "\tslt $t3, $v0, $t8\n"
	    "\tbeq $t3, $zero $skipif%d\n"
	    "\tnop\t",ifc);
    break;
  case NOTEQ: // !=
    fprintf(rfp,
	    "\tsub $t3, $t8, $v0\n"
	    "\tbeq $t3, $zero, $skipif%d\n"
	    "\tnop\n",ifc);
    break;
  case LESSEQ: // <=
    fprintf(rfp,
	    "\tslt $t3, $v0, $t8\n"
	    "\tbne $t3, $zero, $skipif%d\n"
	    "\tnop\n",ifc);
    break;
  case GREATEQ: // >=
    fprintf(rfp,
	    "\tslt $t3, $t8, $v0\n"
	    "\tbne $t3, $zero $skipif%d\n"
	    "\tnop\n",ifc);
    break;
  default:
    printf("error ifの比較演算子\n");
    break;
  }

  token = nextToken(fp);
  if(token->type != KAKKO1R) exit(1);//(<条件式>)
  
  token = nextToken(fp);             // {
  if(token->type != KAKKO3L) exit(1);
  bunshu(fp);
  token = nextToken(fp);
  if(token->type != KAKKO3R) exit(1);//{<文集合>}

  fprintf(rfp,"$skipif%d:\n",ifc);    // if処理終了

  token = nextToken(fp);
  if(token->type == ELSE){//ELSE
    fprintf(rfp,"$else%d:\n",ifc);
    token = nextToken(fp);
    if(token->type != KAKKO3L) exit(1);
    if(ifope == LESS || ifope == GREAT || ifope == NOTEQ){
      fprintf(rfp,
	      "\tbne $t3, $zero, $ifend%d\n"
	      "\tnop\n",ifc);
    }else{
      fprintf(rfp,
	      "\tbeq $t3, $zero, $ifend%d\n"
	      "\tnop\n",ifc);
    }
    bunshu(fp);
    token = nextToken(fp);
    if(token->type != KAKKO3R) exit(1);//{<文集合>}
  }
  else ungetToken();

  fprintf(rfp,"$ifend%d:\n",ifc);

  printf("if文終了\n");
}

// 代入文の解析
// 文中のやつ
void dainyubun(FILE *fp){
  TokenSt *token;
  int x;

  printf("代入文開始\n");

  token = nextToken(fp);
  if(token->type != IDENT) exit(1);
  fprintf(rfp,"\tla $t2, %s       #変数のアドレスを格納\n",token->string);

  for(x=0;strcmp(id[x].name, token->string);x++){
    printf("%s %s\n",id[x].name,token->string);
    if(x==vi){
      printf("error %s 宣言されていない識別子です\n",token->string);
      exit(1);
    }
  }

  token = nextToken(fp);
  if(token->type == KAKKO2L){
    ungetToken();
    soejiretsu(fp, x);
    token = nextToken(fp);
  }

  switch(token->type){
  case INCRE:
    fprintf(rfp,
	    "\tlw $v0, 0($t2)\n"
	    "\tnop\n"
	    "\taddi $v0, $v0, 1\n"
	    "\tsw $v0, 0($t2)\n"
	    "\taddi $sp, $sp, 4\n");
    break;
  case DECRE:
    fprintf(rfp,
	    "\tlw $v0, 0($t2)\n"
	    "\tnop\n"
	    "\taddi $v0, $v0, -1\n"
	    "\tsw $v0, 0($t2)\n"
	    "\taddi $sp, $sp, 4\n");
    break;
  case EQUAL:
    printTree(Oparser(fp));
    fprintf(rfp,
	    "\tsw $v0, 0($t2)    #変数の値を更新\n"
	    "\taddi $sp, $sp, 4\n");
    break;
  default: 
    exit(1);
  }
  token = nextToken(fp);
  if(token->type != SEMICOLON) exit(1);
  printf("代入文終了\n");
}

// 関数文の解析
void kansubun(FILE *fp){
  TokenSt *token;
  printf("関数文開始\n");

  token = nextToken(fp);  //関数名 
  token = nextToken(fp);  //左括弧 (
  while(1){
    jflag=1;
    Oparser(fp);
    jflag=0;
    token = nextToken(fp);
    if(token->type != COMMA) break;
  }

  if(token->type != KAKKO1R) exit(1);
  token = nextToken(fp);
  if(token->type != SEMICOLON) exit(1);
  
  printf("関数文終了\n");
}

// 条件式の解析
void jokenshiki(FILE *fp){
  TokenSt *token;
  int lgc = lgcount-1;
 
  printf("条件式開始\n");

  //  Oparser(fp);
  token = nextToken(fp);
  strcpy(looptable[lgc].string,token->string);

  token = nextToken(fp);
   
  switch(token->type){
  case EQUAL2:
    looptable[lgc].flag=1;
    break;
  case LESS:
    looptable[lgc].flag=2;
    break;
  case GREAT:
    looptable[lgc].flag=3;
    break;
  case NOTEQ:
    looptable[lgc].flag=4;
    break;
  case LESSEQ:
    looptable[lgc].flag=5;
    break;
  case GREATEQ:
    looptable[lgc].flag=6;
    break;
  default:
    printf("error: jokenshiki\n");
    exit(1);
  }

  jflag=1;
  looptable[lgc].node = Oparser(fp);
  jflag=0;

  printf("条件式終了\n");
}

// 代入式
// forとかの中に入るやつ
void dainyushiki(FILE *fp, int i){
  TokenSt *token;
  int x;

  printf("代入式開始\n");

  token = nextToken(fp);
  if(token->type == IDENT){
    for(x=1;x!=vi;x++){
      if(strcmp(id[x].name, token->string))break;
      if(x==vi){
	printf("error 宣言されていないない識別子です\n");
	exit(1);
      }
    }
    strcpy(looptable[i].forstring,token->string);
    
    token = nextToken(fp);
    if(token->type == KAKKO2L){
      ungetToken();
      soejiretsu(fp, x);
      token = nextToken(fp);
    }
    
    switch(token->type){
    case INCRE:
      looptable[i].forflag=1;
      break;
    case DECRE:
      looptable[i].forflag=2;
      break;
    case EQUAL:
      looptable[i].forflag=3;
      jflag=1;
      looptable[i].fornode=Oparser(fp);
      jflag=0;
      break;
    default: 
      printf("error for第三式\n");
      exit(1);
    }
  }
    else exit(1);
  printf("代入式終了\n");
}
