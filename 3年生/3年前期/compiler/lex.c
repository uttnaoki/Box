#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "define.h"

static StateType table[][12] = {
  /* delim, zero, number, alpha, kugiri, add, minus, timesdiv, equal, greatless, not, error */
  {Init, Int, Int, Ident, Kugiri, Add, Minus, Timesdiv, Comp1, Comp1, Not, Error}, /* Init */
  {Final, Error, Error, Error, Final, Final, Final, Final, Final, Final, Final, Error}, /* Zero*/
  {Final, Int, Int, Error, Final, Final, Final, Final, Final, Final, Final, Error}, /* Int */
  {Final, Ident, Ident, Ident, Final, Final, Final, Final, Final, Final, Final, Error}, /* Ident */
  {Final, Final, Final, Final, Final, Final, Final, Final, Comp2, Final, Final, Error}, /* Comp1 */
  {Final, Final, Final, Final, Final, Final, Final, Final, Final, Final, Final, Error}, /* Comp2 */
  {Final, Error, Error, Error, Error, Error, Error, Error, Noteq, Error, Error, Error}, /* Not */
  {Final, Final, Final, Final, Final, Final, Final, Final, Final, Final, Final, Error}, /* Noteq */
  {Final, Final, Final, Final, Final, Final, Final, Final, Final, Final, Final, Error}, /* Kugiri */
  {Final, Final, Final, Final, Final, Zougen, Final, Final, Final, Final, Final, Error}, /* Add */
  {Final, Final, Final, Final, Final, Final, Zougen, Final, Final, Final, Final, Error}, /* Minus */
  {Final, Final, Final, Final, Final, Final, Final, Final, Final, Final, Final, Error},/* Zougen */
  {Final, Final, Final, Final, Final, Final, Final, Final, Final, Final, Final, Error}/* Timesdiv*/
};
/*static int flag = 0; */

static CharType CharToCharType(int c);

static TokenType whichTokenType(char *s, StateType state);

static int flag;
static TokenSt *previous;

TokenSt *nextToken(FILE *fp){
  static char	FIFO[TOKENMAX];
  TokenSt	*token = NULL;
  StateType	state, nstate, pstate;
  int i;
  char c;
  CharType ct;

    if(flag == 1){
    flag = 0;
    return previous;
    }
  /*1文字ずつ読み込んで状態遷移する*/
  state = Init;
  i = 0;
  while(state != Final){
    if (feof(fp)) return NULL;
    c = getc(fp);
    ct = CharToCharType(c);
    pstate = state;
    nstate = table[state][ct];
    state = nstate;
    if((nstate != Final) && (ct != delim))
      FIFO[i++] = c;
  }
  ungetc(c, fp);
  FIFO[i] = '\0';
  token = (TokenSt *)malloc(sizeof(TokenSt));
  strcpy(token->string, FIFO);
  token->type = whichTokenType(token->string, pstate);
  previous = token;
  return token;
}


/*--< 文字を入力とし,文字の種類を返す関数 >--*/
static CharType CharToCharType(int c){
  if((c == ' ') || (c == '\t') || (c == '\n') || (c == EOF)) return delim;
  if(c == '0') return zero;
  if((c >= '1') && (c <= '9')) return number;
  if(((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'))) return alpha;
  if((c == ';') || (c == '(') || (c == ')') || (c == '[') || (c == ']') || (c == '{') || (c == '}') || (c == ',')) return kugiri;
  if(c == '+') return add;
  if(c == '-') return minus;
  if((c == '*') || (c == '/')) return timesdiv;
  if(c == '=') return equal;
  if((c == '>') || (c == '<')) return greatless;
  if(c == '!') return not;
  fprintf(stderr, "Error: undefind character:%c\n", (int)c);
  exit(1);   
}

/*--< 直前の状態と(トークンの)文字列を入力とし,トークンの種類を返す関数 >--*/
static TokenType whichTokenType(char *s, StateType state){
   switch(state){
 
   case Zero:
     return INTEGER;
     break;
     
   case Int:
     return INTEGER;
     break;
     
   case Ident:
     if(strcmp(s,"define")==0) return DEFINE;
     else if(strcmp(s,"while")==0) return WHILE;
     else if(strcmp(s,"for")==0) return FOR;
     else if(strcmp(s,"if")==0) return IF;
     else if(strcmp(s,"else")==0) return ELSE;
     else if(strcmp(s,"break")==0) return BREAK;
     else return IDENT;
     break;
     
   case Add:
     return ADD;
     break;
     
   case Minus:
     return MINUS;
     break;
     
   case Timesdiv:
     if(*s=='*') return TIMES;
     else if(*s=='/') return DIVIDE;
     break;
     
   case Comp1:
     if(*s=='=') return EQUAL;
     else if(*s=='>') return GREAT;
     else if(*s=='<') return LESS;
     break;
     
   case Comp2:
     if(*s=='>') return GREATEQ;
     else if(*s=='<') return LESSEQ;
     else if(*s=='=') return EQUAL2;
     break;
     
   case Noteq:
     return NOTEQ;
     break;
     
   case Zougen:
     if(*s=='+') return INCRE;
     else if(*s=='-') return DECRE;
     break;
     
   case Kugiri:
     if(*s=='(') return KAKKO1L;
     else if(*s==')') return KAKKO1R;
     else if(*s=='[') return KAKKO2L;
     else if(*s==']') return KAKKO2R;
     else if(*s=='{') return KAKKO3L;
     else if(*s=='}') return KAKKO3R;
     else if(*s==';') return SEMICOLON;
     else if(*s==',') return COMMA;
     break;
   default:
     exit(1);
   }
}

void ungetToken(void){
  flag = 1;
}
