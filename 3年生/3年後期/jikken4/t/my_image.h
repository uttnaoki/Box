#pragma once

#include<stdio.h>
#include<stdlib.h>

typedef struct _Image {
  unsigned char*data;
  int W,H;
} Image;


//H行W列の行列、data[]で要素を指定
typedef struct {
  double *data;
  int W,H;
} Matrix;




Image*ImageAlloc(int _W,int _H);
Image*ImageRead(const char*name);
void ImageFree(Image*im);
void ImageWrite(const char*name,Image*im);
void ImageClear(Image*im);



#define IElem(_im,_x,_y,_c) (_im) -> data[(_y)*(_im)->W*3 +(_x)*3+(_c)]
#define isInsideImage(is,u,v) (((unsigned)u<is->W)&&((unsigned)v<is->H))
#define DElem(_a,_c,_b)  (_a)->data[(_a)->W*(_b)+(_c)]//自作
#define Elem(_a,_b,_c)  (_a)->data[(_a)->W*(_b)+(_c)]


void ImageImageProjectionAlpha(Image*id,Image*is,double a[3][3],double alpha);
void ImageDrawBox(Image*im,int x,int y);




//空の画像を作成する関数
Image*ImageAlloc(int W,int H){
  Image*im=(Image*)malloc(sizeof(Image));
  im->W=W;
  im->H=H;
  im->data=(unsigned char*)malloc(W*H*3);
  return im;
}




//ppm画像を読み込む関数
Image*ImageRead(const char*name){
  int W,H;
  Image*im;

  FILE*fp=fopen(name,"rb");//バイナリモードで読み込み
  fscanf(fp,"%*s%d%d%*s%*c",&W,&H);
  //`*'を指定すると変数に書き込まれない.WとHに%dの分だけ書き込む
  im=ImageAlloc(W,H);
  fread(im->data,1,W*H*3,fp);
  fclose(fp);
  return im;
}

//行列の領域を確保
Matrix*MatrixAlloc(int _H,int _W){
  Matrix*mt=(Matrix*)malloc(sizeof(Matrix));;
  mt->W = _W,
    mt->H = _H;
  mt->data=(double*)malloc(mt->W*mt->H*sizeof(double));
  return mt;
}



//画像をppmファイルとjpgファイルに出力する関数(自作)
void ImageWrite(const char*name,Image*im){
  char str1[512],str2[512];
  FILE*fp1;
  
  sprintf(str1,"%s.ppm",name);
  sprintf(str2,"cjpeg %s.ppm > %s.jpg",name,name);

  fp1=fopen(str1,"wb");//wbはバイナリモードで書き込み
  
  if(fp1==NULL){     //エラー処理
    fprintf(stderr,"### ERROR!  Cannot open!!###\n\n");   
  }else{
    fprintf(fp1,"P6 %d %d 255\n",im->W,im->H);
    //P6はppmファイルであることを表す
    fwrite(im->data,1,(im->W)*(im->H)*3,fp1);
    fclose(fp1);
    
    system(str2);
  }
}

