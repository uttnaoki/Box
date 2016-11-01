#pragma once

#include<stdio.h>
#include<stdlib.h>

typedef struct _Image {
  unsigned char*data;
  int W,H;
} Image;

typedef struct {
  double *data;
  int W,H;
} Matrix;

#define Elem(_a,_b,_c)  (_a)->data[(_a)->W*(_b)+(_c)]
#define DElem(_a,_b,_c)  (_a)->data[(_a)->W*(_c)+(_b)]
#define Row(_a,_b)     ((_a)->data+(_a)->W*(_b))

Image*ImageAlloc(int _W,int _H);
Image*ImageRead(const char*name);
void ImageFree(Image*im);
void ImageWrite(const char*name,Image*im);
void ImageClear(Image*im);
#define IElem(_im,_x,_y,_c) (_im) -> data[(_y)*(_im)->W*3 +(_x)*3+(_c)]
#define isInsideImage(is,u,v) (((unsigned)u<is->W)&&((unsigned)v<is->H))
void ImageImageProjectionAlpha(Image*id,Image*is,double a[3][3],double alpha);
void mult33(double a[3][3],double b[3][3], double c[3][3]);
void ImageDrawBox(Image*im,int x,int y);
void LSQ(Matrix *tmp,double xy[][2],double uv[][2]);
Matrix*MatrixAlloc(int _H,int _W);
void GREEDY(int N1,int N2,int x1[][2],int x2[][2],Image*im,Image*im2,int w[][4]);
void PANO(Image*im,Image*im2,char*piname,double trance[9]);
void TK(char*image,int x[][2]);
