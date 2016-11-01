/*

gcc -O greedy.c image.c -lm ; ./a.out 0.jpg 2.jpg
*/
#include"image.h"

#define INFINITY (1/0.)

Matrix*MatrixAlloc(int _H,int _W){
  Matrix*mt=(Matrix*)malloc(sizeof(Matrix));;
  mt->W = _W,
  mt->H = _H;
  mt->data=(double*)malloc(mt->W*mt->H*sizeof(double));
  return mt;
}

/*
int Inssort(Matrix*mt,int w[][2]){ //特徴点指標の大きい順に並べる．
}
*/

int matchMethod1(int match[][2],Matrix*mt,
		 Image*im ,int x1[][2],int N1,
		 Image*im2,int x2[][2],int N2){
  int i,j,k,l,n=0;
  int array[30][2];
  double min;
  int x,y;
  
  for(l=0;l<30;l++){
    min=INFINITY;
    for(i=0;i<N1;i++) for(j=0;j<N2;j++) if(Elem(mt,i,j)<min){
	  min = Elem(mt,i,j);
	  x=i;y=j;
	}
    //printf("%f\n",Elem(mt,x,y));
    printf("%d,%d,%d,%d,\n",
	   x1[x][0],x1[x][1],
	   x2[y][0],x2[y][1]);
    for(k=0;k<N1;k++)Elem(mt,k,y)=INFINITY;
    for(k=0;k<N2;k++)Elem(mt,x,k)=INFINITY;
  }
  
  /*
  for(i=0;i<N1;i++){
    double sm=INFINITY,t;
    for(j=0;j<N2;j++){
      t=Elem(mt,i,j);
      // printf("%f ",t);
      if( sm > t ) sm = t, ji=j;
    }
    match[n][0]=i; match[n][1]=ji; n++;
    printf("%d,%d,%d,%d,\n",
	   x1[i][0],x1[i][1],
	   x2[ji][0],x2[ji][1]);
    for(k=0;k<N1;k++) Elem(mt,k,ji) = INFINITY;
  }
  */

  return n;
}

double ImageSSD(Image*im,int x1,int y1, Image*im2,int x2,int y2){
  int i,j,W=7;
  double sr=0,sg=0,sb=0,dr,dg,db;
  for(i=-W;i<=W;i++) for(j=-W;j<=W;j++){
    dr  = IElem(im, x1+j, y1+i, 0) - IElem(im2, x2+j , y2+i, 0);
    dg  = IElem(im, x1+j, y1+i, 1) - IElem(im2, x2+j , y2+i, 1);
    db  = IElem(im, x1+j, y1+i, 2) - IElem(im2, x2+j , y2+i, 2);
    sr += dr*dr;
    sg += dg*dg;
    sb += db*db;
  }
  return sr+sg+sb;
}


void calcSSDtable(Matrix*mt,
		  Image*im ,int x1[][2],int N1,
		  Image*im2,int x2[][2],int N2){
  int i,j;
  for(i=0;i<N1;i++)
    for(j=0;j<N2;j++)
      Elem(mt,i,j) = ImageSSD(im ,x1[i][0],x1[i][1],
			      im2,x2[j][0],x2[j][1]);
}


main(int ac,char**av){
  Image  *im,*im2;
  Matrix *mt;

  int x1[][2]={
#include"0.fea"
  }, N1=30;
  int x2[][2]={
#include"2.fea"
  }, N2=30;

  if(ac<3) return 1;

  im =ImageRead(av[1]);
  im2=ImageRead(av[2]);

  mt=MatrixAlloc(N1,N2);
  calcSSDtable(mt,im,x1,N1,im2,x2,N2);

  {
    int nm,match[999][2];
    nm=matchMethod1(match,mt,im,x1,N1,im2,x2,N2); // 特徴点の対応付け
  }
}
