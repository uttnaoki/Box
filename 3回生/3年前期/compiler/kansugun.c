void 関数群の解析(FILE *fp){
  TokenSt *token;
  
  /* token->type が IDENT の状態でループにはいる */
  /* ループ一回分で関数一つ分を解析 */

  while(1){
    token = nextToken(fp);
    if(token->type != KAKKO1L) exit(1);
    token = nextToken(fp);
    while(1){
      if(token->type != IDENT) break;
      token = nextToken(fp);
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
    if(token->type == DEFINE) sengenbun(fp); // 宣言文の解析
    文集合の解析;
    if(token->type != KAKKO3R) exit(1);  // }
    token = nextToken(fp);
    if(token->type != IDENT) break;     //  識別子でなければ break (終了状態)
  }
