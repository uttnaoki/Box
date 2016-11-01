void hensusengen(FILE *fp){
  TokenSt *token;
  token = nextToken(fp);

  if(token->type==DEFINE) sengenbun(fp);
  // else if 関数群の解析(fp);	
}

void senbenbun(FILE *fp){
  TokenSt *token;

  while(1){
    token = nextToken(fp);
    if(token->type != IDENT){
      printf("error");
      exit(1);
    }
    while(1){
      token = nextToken(fp);
      if (token->type != KAKKO2L) break;
      token = nextToken(fp);
      if(token->type == INTEGER) exit(1);
      token = nextToken(fp);
      if(token->type != KAKKO2R) exit(1);
    }

    if(token->type != SEMICOLON) exit(1);
    token = nextToken(fp);
    if(token->type != DEFINE) break;
  }
}
