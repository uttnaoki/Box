#include"image.h"
#include<math.h>
#include<string.h>

#define __rdtsc() ({ long long a,d; asm volatile ("rdtsc":"=a"(a),"=d"(d)); d<<32|a; })
/*
Matrix*MatrixAlloc(int _H,int _W){
  Matrix*mt=(Matrix*)malloc(sizeof(Matrix));;
  mt->W = _W,
  mt->H = _H;
  mt->data=(double*)malloc(mt->W*mt->H*sizeof(double));
  return mt;
}
*/

void ImageDrawBox(Image*im,int x,int y){
  int u,v,W=7;
  for(v=-W;v<=W;v++) for(u=-W;u<=W;u++){
    IElem(im,x+u,y+v,0)=IElem(im,x+u,y+v,0) + 0xff >> 1;
    IElem(im,x+u,y+v,1)=IElem(im,x+u,y+v,1) >> 1;
    IElem(im,x+u,y+v,2)=IElem(im,x+u,y+v,2) >> 1;
  }
}

void ImageMatrixWrite(Image*im,Matrix*mt,double s){
  int x,y,p;
  for(y=0;y<mt->H;y++) for(x=0;x<mt->W;x++){
    double tmp=mt->data[y*mt->W+x]*s;
    if(tmp>255) tmp=255;
    im->data[(y*im->W+x)*3+0]=
    im->data[(y*im->W+x)*3+1]=
    im->data[(y*im->W+x)*3+2]=tmp;
  }
}

void ImageFeature(Matrix*im2,Image*im){
  int x,y,u,v,W=7,ix,iy;
  double a;
  for(y=W+1;y<im->H-W-1;y++) for(x=W+1;x<im->W-W-1;x++){
    double ixx,ixy,iyy;
    ixx=iyy=ixy=0;
    for(v=-W;v<=W;v++) for(u=-W;u<=W;u++){
      ix=IElem(im, x+u+1, y+v, 1) - IElem(im, x+u-1, y+v, 1);
      iy=IElem(im, x+u, y+v+1, 1) - IElem(im, x+u, y+v-1, 1);
      ixx+=ix*ix; // ixx だけでなく ixy,iyy も計算する．
      ixy+=ix*iy;
      iyy+=iy*iy;
    }
    a=((ixx+iyy)-sqrt(pow(ixx+iyy,2)-4*(ixx*iyy-pow(ixy,2))))/2;
    DElem(im2,x,y)=a; // 実際には [ixx,ixy;ixy,iyy] の小さい方の固有値を入れる．
  }
}

int Inssort(Matrix*im2,int w[][2],int a){ //特徴点指標の大きい順に並べる．
  int i,j,t,b,c;
  for(i=0;i<a;i++){
    t=DElem(im2,w[i][0],w[i][1]);
    b=w[i][0];
    c=w[i][1];
    j=i;
    for(;j>=1 && DElem(im2,w[j-1][0],w[j-1][1])<t; j--) {
      w[j][0]=w[j-1][0];
      w[j][1]=w[j-1][1];
    }
    w[j][0]=b;
    w[j][1]=c;
  }
}

int MatrixLocalMax(int w[][2], Matrix*im2){
  int x,y,u,v,W=7,n=0;
  for(y=W+1;y<im2->H-W-1;y++) for(x=W+1;x<im2->W-W-1;x++){
      double max=-1;
      for(v=-W;v<=W;v++) for(u=-W;u<=W;u++){
	  if(max<DElem(im2,x+u,y+v)){
	      max = DElem(im2,x+u,y+v);
	    }// (x,y) を中心とする 15x15 の矩形領域内で DElem(im2,x+u,y+v) の最大値を探す．
	}
      if(DElem(im2,x,y)==max){
	w[n][0]=x;
	w[n][1]=y;
	n++;// 最大値が DElem(im2,x,y) と等しいなら，(x,y) を特徴点として記録する．
      }
    }
   Inssort(im2,w,n);
  return n; // 記録した点の数
}
