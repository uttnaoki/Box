#define TOKENMAX 100
#define MaxStack 128

/* 文字のタイプ */
typedef enum{
  delim,         /* 区切り記号(空白,TAB,改行) */
  zero,          /* 0 */
  number,        /* 数字　1~9 */
  alpha,         /* 英字　a~z,A~Z */
  kugiri,        /* ;,(,),[,],{,} */
  add,           /* 加算 + */
  minus,         /* 減算 - */
  timesdiv,      /* 乗除算　*,/ */
  equal,         /* 等号 = */
  greatless,     /* >,< */
  not,           /* ! */
  error,         /* 未定義文字 */
} CharType;

/* 状態のタイプ */
typedef enum{
  Init,          /* 初期状態 */
  Zero,          /* 0状態 */
  Int,           /* 数状態 */
  Ident,         /* 識別子状態 */
  Comp1,         /* 単一比較状態 */
  Comp2,         /* 二重比較状態 */
  Not,           /* NOT状態 */
  Noteq,         /* NOT EQ状態 */
  Kugiri,        /* 区切り文字状態 */
  Add,           /* 加算状態 */
  Minus,         /* 減算状態 */
  Zougen,        /* 増減演算子状態 */
  Timesdiv,      /* 乗除演算子状態 */
  Final,         /* 終了状態 */
  Error,         /* エラー状態 */
} StateType;

/* トークンのタイプ */
typedef enum{
  INTEGER,       /* 整数 00 */
  IDENT,         /* 識別子 01 */
  ADD,           /* 加算演算子 + 02 */
  MINUS,         /* 減算演算子 - 03 */
  TIMES,         /* 乗算演算子 * 04 */
  DIVIDE,        /* 除算演算子 / 05 */
  EQUAL,         /* 等号 = 06 */
  GREAT,         /* > 07 */
  LESS,          /* < 08 */
  EQUAL2,        /* == 09 */
  GREATEQ,       /* >= 10 */
  LESSEQ,        /* <= 11 */
  NOTEQ,         /* != 12 */
  INCRE,         /* ++ 13 */
  DECRE,         /* -- 14 */
  KAKKO1L,       /* ( 15 */
  KAKKO1R,       /* ) 16 */
  KAKKO2L,       /* [ 17 */
  KAKKO2R,       /* ] 18 */
  KAKKO3L,       /* { 19 */
  KAKKO3R,       /* } 20 */
  SEMICOLON,     /* ; 21 */
  DEFINE,        /* 予約語(define) 22 */
  WHILE,         /* 予約語(while) 23 */
  FOR,           /* 予約語(for) 24 */
  IF,            /* 予約語(if) 25 */
  ELSE,          /*予約後(else)26*/
  SPACE,         /*  予約語(‘　’)27 */
  TAB,            /*  予約語(\t)28 */
  ENTER,          /*  予約語(\n)29 */
  DOL,
  ERR,          /* error */
  COMMA,        /*  ,  */
  BREAK,
} TokenType;

/*----演算子の型を列挙----*/
typedef enum {
  ot_PlusMinus, /* ＋，－ */
  ot_MultDiv,   /* ＊，／ */
  ot_LPar,      /*  （    */
  ot_RPar,       /*   )   */
  ot_LPar2,     /*   [    */
  ot_RPar2,     /*   ]    */
  ot_dollar     /*   $   */
} OpeType;

/* トークンを格納する構造体 */
typedef struct{
  char string[TOKENMAX]; /* トークンの文字列 */
  TokenType type; /* トークンのタイプ */
} TokenSt;

typedef struct node {
  TokenSt *token;              
  struct node *left;
  struct node *right;
  struct node *arg[TOKENMAX];
}Node;

typedef struct {
  char name[TOKENMAX];
  int addr;
  int next;
  int subcount;       //添字[]の数
  int subamount[TOKENMAX];    //各添字の容量
  int addramount[TOKENMAX];   //アドレス計算で使用
}Vartable;            //変数

typedef struct{
  char name[TOKENMAX];
}Functable;

typedef struct{
  int flag;       
  Node *node;
  char string[100];
  //  char sub[100][100];
  int forflag;
  Node *fornode;
  char forstring[100];
  //  char forsub[100][100];
}Looptable;

TokenSt *nextToken(FILE *);
void ungetToken();
int jflag;
int argflag;
int freeflag;
void printTree(Node *node);
void freeTree(Node *node);
FILE *rfp;
