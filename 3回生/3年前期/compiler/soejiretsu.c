void soejiretsu(FILE *fp){
  TokenSt *token;

  while(1){
    token = nextToken(fp);
    if(token->type == IDENT){
      token = nextToken(fp);
      if(token->type == KAKKO2L) soejiretsu(fp);
    }
    else if(token->type != INTEGER) exit(1);
    
    token = nextToken(fp);
    if(token->type != KAKKO2R) exit(1);
    
    token = nextToken(fp);
    if(token->type != KAKKO2L) {
      ungetToken();
      break;
    }
  }
}
