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


void MatrixFree(Matrix*mt){
  free(mt->data);
  free(mt);
}

int matchMethod(int w[][4],Matrix*mt,
		 Image*im ,int x1[][2],int N1,
		 Image*im2,int x2[][2],int N2){
  int i,j,k,l,n=0;
  double min;
  int x,y;
  //for(i=0;i < MAX;i++) rndAry[i]=i;
  
  for(l=0;l<30;l++){
    min=INFINITY;
    for(i=0;i<N1;i++) for(j=0;j<N2;j++) if(Elem(mt,i,j)<min){
	  min = Elem(mt,i,j);
	  x=i;y=j;
	}
    /*
    printf("%d,%d,%d,%d,\n",
	   x1[x][0],x1[x][1],
	   x2[y][0],x2[y][1]);
    */
    w[l][0]=x1[x][0];
    w[l][1]=x1[x][1];
    w[l][2]=x2[y][0];
    w[l][3]=x2[y][1];
    n++;

    for(k=0;k<N1;k++)Elem(mt,k,y)=INFINITY;
    for(k=0;k<N2;k++)Elem(mt,x,k)=INFINITY;
  } 
  return n;
}
/*
int count_feature(Matrix *aa,int w[][4]){
  int i,j;
  double x,y;
  double u,v;
  int n=0;
  double r,z=1;
  double dx,dy;

  for(i=0;i<30;i++){
    x=w[i][0];y=w[i][1];
    //r = 1 / (Elem(aa,0,6)*x+Elem(aa,0,7)*y+z);
    u = (Elem(aa,0,0)*x+Elem(aa,0,1)*y+Elem(aa,0,2));
    v = (Elem(aa,0,3)*x+Elem(aa,0,4)*y+Elem(aa,0,5));
    dx=u-w[i][2];dy=v-w[i][3];
    if((dx*dx+dy*dy)<1) n++;
  }
  return n;
}
*/
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
/*
void RANSAC(int rndAry[MAX]){
  int i;
  for(i=0;i < 4;i++){
  int j, t;
  j=(int)(drand48()*MAX);
  t=rndAry[i]; rndAry[i]=rndAry[j]; rndAry[j]=t;
  }
}
*/
void GREEDY(int N1,int N2,int x1[][2],int x2[][2],Image*im,Image*im2,int w[][4]){
  Matrix*mt;
  //int i,j,k,l;
  //double xy[4][2],uv[4][2];
  //int maxf=0;
  //int cf;
  //double bb[8];
  //Matrix*aa;
  //aa=MatrixAlloc(1,8);
  mt=MatrixAlloc(N1,N2);
  calcSSDtable(mt,im,x1,N1,im2,x2,N2);
  {
    int nm,match[999][2];
    nm=matchMethod(w,mt,im,x1,N1,im2,x2,N2); // 特徴点の対応付け
  }
}

//gcc -O main.c greedy.c image.c lsq.c -lm ; ./a.out 0.jpg 2.jpg
//gcc -O pano0.c image.c && ./a.out out.jpg && display out.jpg&
