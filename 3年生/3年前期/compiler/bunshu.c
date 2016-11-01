void bunshu(FILE *fp){
  TokenSt *token;
  
  token = nextToken(fp);

  while(1){
    if(token->type == WHILE){
      whilebun(fp);
    }else if (token->type == FOR){
      forbun(fp);
    }else if(token->type == IF){
      ifbun(fp);
    }else if (token->type==IDENT){
      token=nextToken(fp);
      if(token->type == KAKKO1L) kansubun(fp);
      else dainyubun;
    }
    else break;
  }
}

void whilebun(FILE *fp){

  TokenSt *token;

  token = nextToken(fp);
  if(token->type != KAKKO1L) exit(1);
  jokenshiki;
  token = nextToken(fp);
  if(token->type != KAKKO1R)　exit(1);　//(<条件式>)

  token = nextToken(fp); 
  if(token->type != KAKKO3L)　exit(1);
  文集合の解析;
  token = nextToken(fp);
  if(token->type != KAKKO3R)　exit(1); //{<文集合>}
}

void forbun(FILE *fp){
  
  TokenSt *token;

  token = nextToken(fp);
  if(token->type != KAKKO1L) exit(1);
  token = nextToken(fp);
  if(token->type != IDENT) exit(1);
  token = nextToken(fp);
  if(token->type == KAKKO2L){
    soejiretsu(fp);
    token = nextToken(fp);
    if(token->type != EQUAL) exit(1);
    token = nextToken(fp);
  }else if(token->type == EQUAL){
    token = nextToken(fp);
  }else exit(1);

  if(token->type != INTEGER)exit(1);

  token = nextToken(fp);
  if(token->type != SEMICOLON) exit(1);
  jokenshiki(fp);
  token = nextToken(fp); 
 　if(token->type != SEMICOLON) exit(1);
 dainyushiki(fp);
  token = nextToken(fp); 
  if(token->type != KAKKO1R) exit(1);　//(<代入文>;<条件式>;<代入式>)

  token = nextToken(fp);
  if(token->type != KAKKO3L) exit(1);
  bunshu(fp);
  token = nextToken(fp);
  if(token->type != KAKKO3R) exit(1);　//{<文集合>}
}

void ifbun(FILE *fp){

  TokenSt *token;

  token = nextToken(fp);
  if(token->type != KAKKO1L) exit(1);
  jokenshiki(fp);
  token = nextToken(fp);
  if(token->type != KAKKO1R) exit(1);//(<条件式>)

  token = nextToken(fp);
  if(token->type != KAKKO3L) exit(1);
  bunshu(fp);
  token = nextToken(fp);
  if(token->type != KAKKO3R) exit(1);//{<文集合>}

  token = nextToken(fp);
  if(token->type == ELSE){//ELSE

  token = nextToken(fp);
  if(token->type != KAKKO3L) exit(1);
  bunshu(fp);
  token = nextToken(fp);
  if(token->type != KAKKO3R) exit(1);//{<文集合>}
  }
}
  
  
void dainyubun(FILE *fp){

  TokenSt *token;

  if(token->type == KAKKO2L){
    soejiretsu(fp);
  }
  token = nextToken;

  switch(token->type){
  case INCRE:
    break;
  case DECRE:
    break;
  case EQUAL:
    oparser(fp);
    break;
  default: 
    exit(1);
  }
/* if(token->type !=INCRE||token->type != DECRE||token->type !=EQUAL2)exit(1); 
   if(token->type == EQUAL2)  oparser(fp);//スペルあってる？*/
  
  token = nextToken;
  if(token->type != SEMICOLON) exit(1);
    
}

void kansubun(FILE *fp){

  TokenSt *token;

  token = nextToken(fp);

  while(1){
  oparser(fp);
  token = nextToken(fp);
  if(token->type != COMMA) break; 
  }

  if(token->type != KAKKO1R) exit(1);
  token = nextToken(fp);
  if(token->type != SEMICOLON) exit(1);
  
}



void jokenshiki(FILE *fp){

  TokenSt *token;

  oparser(fp);
  token = nextToken(fp);

  if(token->type !=EQUAL2||token->type != LESS||token->type !=GREAT||
  token->type !=NOTEQ||token->type != LESSEQ||token->type !=GREATEQ) exit(1);
  oparser(fp);
}

void dainyushiki(FILE *fp){

  TokenSt *token;
  token = nextToken(fp);
  if(token->type == IDENT){
    token = nextToken(fp);
    if(token->type == KAKKO2L){
      soejiretsu(fp);
      token = nextToken(fp);
    }
    
    switch(token->type){
    case INCRE:
      break;
    case DECRE:
      break;
    case EQUAL:
      oparser(fp);
      break;
    default: 
      exit(1);
    }
  }else if(token->type ==INCRE||token->type ==DECRE ){
    token = nextToken(fp);
    if(token->type == IDENT){
      token = nextToken(fp);
      if(token->type == KAKKO2L){
	soejiretsu(fp);
      }
    }
    else exit(1);
  }
}
