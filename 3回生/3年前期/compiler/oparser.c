#include<stdio.h>
#include <stdlib.h>
#include<string.h>
#include "define.h"

#define GREATER 1
#define LESS -1
#define Equal 0
#define ERR 9
#define FIN 5

#define MAXMATRIX 100

TokenSt *nextToken(FILE *fp);

extern Vartable id[TOKENMAX];
extern int vi;

static Node *Stack[2][MaxStack];
static int Sptr[2];
int num;
int n =0;
int joukenshiki=0;  

void ungetToken(void);
void printop(Node *node);
void freeTree(Node *node);
void pop2();
void push2();
     
static int orderMatrix[][7]={
  /* +-, * /, (, ), [, ], $ */
  /*+-*/
  { GREATER, LESS, LESS, GREATER, LESS, ERR, GREATER},
  /* *,/ */
  { GREATER, GREATER, LESS, GREATER, LESS, ERR, GREATER},
  /* ( */
  { LESS, LESS, LESS, Equal, LESS, LESS, ERR},
  /* ) */
  { GREATER, GREATER, ERR, GREATER, ERR, ERR, GREATER},
  /* [ */
  { ERR, ERR, ERR, ERR, LESS, Equal, ERR},
  /* ] */
  {GREATER, GREATER, ERR, ERR, LESS, LESS, GREATER},
  /* $ */
  { LESS, LESS, LESS, ERR, LESS, ERR, FIN}
};
     
static OpeType typeToOpeType(TokenType type){
  if((type == ADD)||(type == MINUS)) return ot_PlusMinus;
  else if((type == TIMES)||(type == DIVIDE)) return ot_MultDiv;
  else if(type == KAKKO1L) return ot_LPar;
  else if(type == KAKKO1R) return ot_RPar;
  else if(type == KAKKO2L) return ot_LPar2;
  else if(type == KAKKO2R) return ot_RPar2;
  else if(type == DOL) return ot_dollar;
  else return error;
}
     
void push(int S, Node *n){
  if (Sptr[S]>=MaxStack){// エラー処理
    fprintf(stderr, "Stack_%d overflow.\n",S);
    exit(1);
  }else{
    Stack[S][Sptr[S]] = n;
    Sptr[S] = Sptr[S] + 1;
  }
  printf("push(%d,'%s')\n",S, n->token->string);
}
     
Node* pop(int S){ /* スタックS をpop する*/
  if (Sptr[S]<=0){ // エラー処理
    fprintf(stderr,"Stack_%d underflow.\n",S);
    exit(1);
  }else{
    Sptr[S] = Sptr[S] - 1;
    printf("pop(%d,'%s')\n",S, Stack[S][Sptr[S]]->token->string);
    return Stack[S][Sptr[S]];
  }
}
     
Node *Top(){ /* 演算子スタックの最上部のデータの優先順位を返す*/
  if (Sptr[1]==0){ //エラー処理
    fprintf(stderr,"Stack_1 empty.\n");
    exit(1);
  }else return Stack[1][Sptr[1]-1];
}
     
int Check(Node *Operator, int i){
  Node *N, *node;
  Node *topNode;
  OpeType order;
  
  topNode = Top();
  order = orderMatrix[ typeToOpeType(topNode->token->type) ]
    [ typeToOpeType(Operator->token->type) ];

  if(order == FIN) {
    printf("fin\n");
    ungetToken();
    return 1; /* 算術式解析の終了 */
  }

  /* if(joukenshiki){
    printf("fin\n");
    ungetToken();
    printf("%s aaaaa\n",Operator->token->string);
    freeTree(Operator);
    if(Sptr[0] >= 2 && Sptr[1] >= 2){
      N=pop(1);
      N->right=pop(0);
      N->left=pop(0);
      push(0,N);
    }
    joukenshiki=0;
    return 1;
    }*/

  if(order == ERR){
    printf("error\n");
    ungetToken();
    return 1; /* 算術式解析の終了 */
  }
  if(typeToOpeType(Operator->token->type) != ot_LPar2 &&
     typeToOpeType(Operator->token->type) != ot_RPar2) num = 0;
     
  if ( Sptr[1]==0 || order == LESS){
    push(1,Operator);
    return 0; /* まだ終了していない */
    
  }else if(typeToOpeType(topNode->token->type) == ot_LPar ){
    pop(1);
    return 0;
  }else if(typeToOpeType(topNode->token->type) == ot_LPar2){
    printf("配列処理 oparser\n");
    if(n == 0){
      node = NULL;
      strcpy(node->token->string, "0");
      push(0,node);
    }
    N = pop(0);
    push(1,N);
    N = pop(0);
    N->arg[i] = pop(1);
    push(0,N);
    pop(1);
    num++;
    return 0;
   
  }else{
    N=pop(1);
    N->left=pop(0);
    N->right=pop(0);
    
    push(0,N);
    return Check(Operator,num); //< 再帰的処理．その結果を受け取り，戻り値とする>
  }
}

Node* Oparser(FILE *fp){
  TokenSt *token;
  Node *node;
  int kflag=1,kkflag=1;
  int final;
   
  num=0;
  final = 0; 
  Sptr[0] = 0; Sptr[1] = 0; /*スタックポインタ初期化*/
  node = (Node *)malloc(sizeof(Node)); /*$をつっこんどく*/
  node -> token = (TokenSt *)malloc(sizeof(TokenSt));
  node -> token -> type = DOL;
  node -> token -> string[0] = '$';
  node -> token -> string[1] = '\0';
  push(1,node);
     
  while(final==0){
    token = nextToken(fp);/* 1字句入力*/

  printf("%s\n",token->string);

    node = (Node *)malloc(sizeof(Node));
    node->token = token;
    node->left = NULL;
    node->right= NULL;
    memset(node->arg,0,sizeof(node->arg));
    if(node->token->type == INTEGER ||
       node->token->type == IDENT ){
      push(0,node); /*数値と識別子はオペランド用のスタックにプッシュ*/
      n = 1;
    }
    else if(node->token->type == ADD || node->token->type == MINUS ||
	    node->token->type == TIMES || node->token->type == DIVIDE ||
	    node->token->type == DOL){
      final = Check(node, num);
      /*演算子の場合の処理*/
    }
    else if(node->token->type == KAKKO1L){
      final = Check(node, num);
      kflag++;
    } else if(node->token->type == KAKKO1R){
      kflag--;
      if(kflag==0 && jflag==1){
	joukenshiki=1;
	ungetToken();
	node = (Node *)malloc(sizeof(Node));
	node -> token = (TokenSt *)malloc(sizeof(TokenSt));
	node -> token -> type = DOL;
	node -> token -> string[0] = '$';
	node -> token -> string[1] = '\0';
      }
      final = Check(node, num);
    }else if(node->token->type == KAKKO2L){
      final = Check(node, num);
      kkflag++;
    }else if(node->token->type == KAKKO2R){
      kkflag--;
      if(kkflag==0 && argflag==1){
	joukenshiki=1;
	ungetToken();
	node = (Node *)malloc(sizeof(Node));
	node -> token = (TokenSt *)malloc(sizeof(TokenSt));
	node -> token -> type = DOL;
	node -> token -> string[0] = '$';
	node -> token -> string[1] = '\0';
      }
      final = Check(node, num);
    }else{
      ungetToken();
      node = (Node *)malloc(sizeof(Node));
      node -> token = (TokenSt *)malloc(sizeof(TokenSt));
      node -> token -> type = DOL;
      node -> token -> string[0] = '$';
      node -> token -> string[1] = '\0';
      final=Check(node, num);
    }
  }
  return Stack[0][0];
}

int caladdr(int x, int suba[]){   //算術式中の配列が格納されたアドレス計算
  int m=0, addr, n;

  addr=id[x].addr;
  n=id[x].subcount; 
  while(n>0){
    if(suba[m] >= id[x].subamount[m]){   //算術式中と宣言部での添字の整合性判定
      printf("error subscript\n");
      exit(1);
    }
    addr = addr + suba[m] * id[x].addramount[m];
    m++;
    n--;
  }
  return addr;
}

void printTree(Node *node){
  int x,i;
  int n;
  
  // fprintf(rfp,"#printTree %s",node->token->string);
  // fprintf(rfp,"\n");
  //  fprintf(rfp,"%s\n",node->left->token->string);
  //fprintf(rfp,"%s\n",node->right->token->string);

  if(node->left != NULL && node->right != NULL) {
    printTree(node->left);
    printTree(node->right);
  }

  if(node->token->type == INTEGER){
    fprintf(rfp,"\tli $v0, %s", node->token->string);
    fprintf(rfp,"\t#整数");
    fprintf(rfp,"\n");
    push2();
  }
  else if(node->token->type == IDENT){
    //    fprintf(rfp,"\tla $v0, %s", node->token->string);
    //fprintf(rfp,"        #変数");
    //fprintf(rfp,"\n");
    if(node->arg[0] != NULL){   
      fprintf(rfp,"\tadd $t5, $zero, $zero        #配列処理\n");
      //配列の識別子が入ったid[x]を特定
      for(x=1;strcmp(id[x].name,node->token->string); x++){
	if(x==vi){
	  printf("error 算術式中の配列に誤りがあります\n");
	  exit(1);
	}
      }
      n=id[x].subcount;
      for(i=0; node->arg[i] != NULL; i++){
	if(n==0){
	  printf("error 算術式中の配列の添字数\n");
	  exit(1);
	}
	printTree(node->arg[i]);
	fprintf(rfp,"\taddi $sp, $sp, 4\n");
	fprintf(rfp,"\tli $t4, %d\n",id[x].addramount[i]);
	fprintf(rfp,"\tmult $v0, $t4\n");
	fprintf(rfp,"\tmflo $t3\n");
	fprintf(rfp,"\tadd $t5, $t5, $t3\n");
      }
      fprintf(rfp,"\tla $v0, %s\n", node->token->string);
      fprintf(rfp,"\tadd $v0, $v0, $t5\n");
      fprintf(rfp,"\tlw $v0, 0($v0)\n");
      fprintf(rfp,"\tnop\n");
      push2();
    }else{
      fprintf(rfp,"\tla $v0, %s\n",node->token->string);
      fprintf(rfp,"\tlw $v0, 0($v0)\n");
      fprintf(rfp,"\tnop\n");
      push2();
    }
  }else{                   //演算子
    fprintf(rfp,"#pop");
    fprintf(rfp,"\n");
    pop2();
    fprintf(rfp,"\n");
    printop(node); 
    fprintf(rfp,"\n");
    push2();
  }
  //if(freeflag)freeTree(node);
}

void printop(Node *node){
  switch(node->token->type){
  case ADD:
    fprintf(rfp,"\tadd $v0, $t0, $t1\n");
    break;
  case MINUS:
    fprintf(rfp,"\tsub $v0, $t0, $t1\n");
    break;
  case TIMES:
    fprintf(rfp,"\tmult $t0, $t1\n");
    fprintf(rfp,"\tmflo $v0\n");
    break;
  case DIVIDE:
    fprintf(rfp,"\tdiv $t0, $t1\n");
    fprintf(rfp,"\tmflo $v0\n");
    break;
  default:
    fprintf(rfp,"%s\n",node->token->string);
    fprintf(rfp,"error\n");
    exit(1);
  }
}

void pop2(){
  fprintf(rfp,"\tlw $t0, 0($sp)\n");
  fprintf(rfp,"\tlw $t1, 4($sp)\n");
  fprintf(rfp,"\tnop\n");
  fprintf(rfp,"\taddi $sp,$sp,8\n");
}

void push2(){
  fprintf(rfp,"\taddi $sp,$sp,-4\n");
  fprintf(rfp,"\tsw $v0,0($sp)\n");
}

void freeTree(Node *node){
   free(node->token);
   free(node);
}
