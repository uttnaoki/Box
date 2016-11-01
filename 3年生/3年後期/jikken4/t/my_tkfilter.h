// gcc TKfilter_qsort2.c -lm -O3 -mavx2 -march=native -funroll-loops -fomit-frame-pointer
// ./a.out 0.ppm



#include"my_image.h"
#include<math.h>
#include <stdlib.h> //qsort用
#include <sys/time.h> 
#include <sys/resource.h>

//#define __rdtsc() ({ long long a,d; asm volatile ("rdtsc":"=a"(a),"=d"(d)); d<<32|a; })
//#define Elem(_a,_b,_c)  (_a)->data[(_a)->W*(_b)+(_c)]
//#define N_max 50 //30~100であればなんでもいい

//my_image.h内のもの
//#define IElem(_im,_x,_y,_c) (_im) -> data[(_y)*(_im)->W*3 +(_x)*3+(_c)]
//#define DElem(_a,_c,_b)  (_a)->data[(_a)->W*(_b)+(_c)]

//double mt22_min_eigenvalue(int a,int b, int c, int d);

typedef struct{
  double x;
  double y;
  double max;
}Array_max;
/*
// getrusage()を使った最大駐在ページサイズの取得と表示 
void use_getrusage(void)
{
	struct rusage r;
	if (getrusage(RUSAGE_SELF, &r) != 0) {
	  //Failure
	}
	printf("maxrss=%ld\n", r.ru_maxrss);
}

*/

//15x15内の領域で各要素を半減する
void ImageDrawBox(Image*im,int x,int y){
  int u,v,W=7;
  for(v=-W;v<=W;v++) for(u=-W;u<=W;u++){
      IElem(im,x+u,y+v,0)=IElem(im,x+u,y+v,0) + 0xff >> 1;//0xffは255のこと
      IElem(im,x+u,y+v,1)=IElem(im,x+u,y+v,1) >> 1;
      IElem(im,x+u,y+v,2)=IElem(im,x+u,y+v,2) >> 1;
      //ここで登場する>>はシフト演算子のこと
      //a>>b でaをbビット分、右へシフトすることを表す
      //右へ1ビットシフトするということは数値が1/2になることを表す
    }
}

//行列mtの要素のs倍を各画素とする画像imをつくる
void ImageMatrixWrite(Image*im,Matrix*mt,double s){
  int x,y;//pを削除
  for(y=0;y<mt->H;y++) for(x=0;x<mt->W;x++){
      double tmp=mt->data[y*mt->W+x]*s;
      if(tmp>255) tmp=255;
      im->data[(y*im->W+x)*3+0]=
	im->data[(y*im->W+x)*3+1]=
	im->data[(y*im->W+x)*3+2]=tmp;
    }
}



//画像imの特徴点の有効さを表すixx,ixy,iyyを計算してim2に格納（改変、自作）
//これらからなる行列の固有値は楕円の長半径・短半径となり、短半径が長いほど適切な特徴点である
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
    DElem(im2,x,y)=a; // [ixx,ixy;ixy,iyy] の小さい方の固有値を入れる．
  }
  }



//im2の中から15x15ごとに最大値を求める（改変、自作）
int MatrixLocalMax(int w[][2], Matrix*im2){
  int x,y,u,v,W=7,n=0,a;
  for(y=W+1;y<im2->H-W-1;y++) for(x=W+1;x<im2->W-W-1;x++){
      double max=-1;
      for(v=-W;v<=W;v++) for(u=-W;u<=W;u++){//iをvに変更
	  if(max<DElem(im2,x+u,y+v))max=DElem(im2,x+u,y+v);// (x,y) を中心とする 15x15 の矩形領域内で DElem(im2,x+u,y+v) の最大値を探す．
	}//15x15内のforここまで
      if(max==DElem(im2,x,y)){//最大値が DElem(im2,x,y) と等しいなら，(x,y) を特徴点として記録する．
	a=n;
	w[a][0]=x;
	w[a][1]=y;
	n++;
      }//w[0]から格納されていく
    }//全体のforここまで
  return n; // 記録した点の数
}



//比較関数 qsort用
int comp( const void *c1, const void *c2 )
{
  Array_max test1 = *(Array_max *)c1;
  Array_max test2 = *(Array_max *)c2;

  double tmp1 = test1.max;   /* max を基準とする */
  double tmp2 = test2.max;

  return (int)(tmp1 - tmp2);
}




//MatrixLocalMaxを変更、クイックソートでN個まで格納
//構造体でソート
//N_max はdefine文で設定
int MatrixLocalMax_qsort2(int w[][2], Matrix*im2){
  int x,y,u,v,W=7,n=0,a,i;
  Array_max temp[N_max];
  for(y=W+1;y<im2->H-W-1;y++) for(x=W+1;x<im2->W-W-1;x++){
      double max=-1;
      for(v=-W;v<=W;v++) for(u=-W;u<=W;u++){
	  if(max<DElem(im2,x+u,y+v))max=DElem(im2,x+u,y+v);// (x,y) を中心とする 15x15 の矩形領域内で DElem(im2,x+u,y+v) の最大値を探す．
	}//15x15内のforここまで
      if(max==DElem(im2,x,y)){//最大値が DElem(im2,x,y) と等しいなら，(x,y) を特徴点として記録する．
	a=n;
	if(a>=N_max){
	  qsort(temp,N_max,sizeof(Array_max),comp);
	  //小さい順にソートし，一番小さいものを上書きする
	  n=N_max-1;
	  if(max>temp[0].max){
	    temp[0].x=x;
	    temp[0].y=y;
	    temp[0].max=max;
	  }
	}else{
	  temp[a].x=x;
	  temp[a].y=y;
	  temp[a].max=max;
	}
	n++;
      }//最大値のifここまで
    }//全体のforここまで
  for(i=0;i<n;i++){//w[][]に先頭を除き小さい順に格納
    w[i][0]=temp[i].x;
    w[i][1]=temp[i].y;
  }
  return n;
}









/*

int main(int ac,char**av){
  Image *im;
  Matrix*im2;
  int kk[N_max][2], kw,i;

  if(ac<2) return 1;

  im=ImageRead(av[1]);
  im2=MatrixAlloc(im->H,im->W);
  ImageFeature(im2,im);
 
  kw=MatrixLocalMax_qsort2(kk,im2);
  
  ImageMatrixWrite(im,im2,.001);
  
  for(i=0;i<kw;i++){ 
    //  if(i>46)fprintf(stderr,"i=%d\n",i);
    ImageDrawBox(im,kk[i][0],kk[i][1]);
  }
  
  ImageWrite("tk_qsort2_out",im);//自作のものppmとjpgを出力


  return 0;
}
*/
