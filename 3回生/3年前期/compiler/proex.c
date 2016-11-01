define x;
define y;

main(){
  define l;
  define m;
  define n;

  x=0;

  while(l<10){
    x=x+1;
    l++;
  }

  for(m=10; m!=0; m--){
    y=y+2;
  }

  mult(x,y);

  if(result == 200){
    x=1;
  }else{
    x=0;
  }
}

mult(a, b){
  result = a*b;
}
