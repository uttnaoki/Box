// cc lsq2.c -lm -O ; ./a.out

#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<string.h>
//#include"mat33.h"

// basic vector operations
double VP(double*a,double*b,int N){
  double s=0;
  int i;
  for(i=0;i<N;i++) s += a[i] * b[i] ;
  return s;
}

void VSS(double*d,double s,int N){
  int i;
  for(i=0;i<N;i++) d[i] *= s;
}

void VSA(double*d,double*a,double s,int N){
  int i;
  for(i=0;i<N;i++) d[i] += a[i] * s;
}

// matrix structure and functions
typedef struct {
  double *data;
  int W,H;
} Matrix;

#define Elem(_a,_b,_c)  (_a)->data[(_a)->W*(_b)+(_c)]
#define Row(_a,_b)     ((_a)->data+(_a)->W*(_b))

Matrix*MatrixAlloc(int _H,int _W){
  Matrix*mt=(Matrix*)malloc(sizeof(Matrix));;
  mt->W = _W,
  mt->H = _H;
  mt->data=(double*)malloc(mt->W*mt->H*sizeof(double));
  return mt;
}

void MatrixClear(Matrix*mt){
  memset(mt->data,0,mt->W*mt->H*sizeof(double));
}


void MatrixCopy(Matrix*mtD,Matrix*mt){
  memmove(mtD->data,mt->data,mt->W*mt->H*sizeof(double));
}

void MatrixCopyT(Matrix*mtD,Matrix*mt){
  int i,j;
  for(i=0;i<mtD->H;i++)
    for(j=0;j<mtD->W;j++)
      Elem(mtD,i,j) = Elem(mt,j,i);
}

void MatrixPrint(Matrix*mt){
  int i,j;
  for(i=0;i<mt->H;i++){
    for(j=0;j<mt->W;j++)
      printf("%f ",Elem(mt,i,j));
    printf("\n");
  }
}

void MatrixMultT(Matrix*mtD,Matrix*mtA,Matrix*mtB){
  // D = A B^T
  int i,j;
  for(i=0;i<mtA->H;i++)
    for(j=0;j<mtB->H;j++)
      Elem(mtD,i,j) = VP( Row(mtA,i), Row(mtB,j), mtA->W);
}


void MatrixQRDecompColMajor(Matrix*mtR,Matrix*mt){
  // gram-schmidt orthonormalization (Lt and Q)
  double t, *aT[] = { Row(mt,0), Row(mt,1), Row(mt,2),
		      Row(mt,3), Row(mt,4), Row(mt,5),
		      Row(mt,6), Row(mt,7)};
  int W = mt->W;
  int H = mt->H;
  int i,j;
  MatrixClear(mtR);
  for(i=0;i<W;i++){
    for(j=0;j<H;j++){
      if(i!=j){
	Elem(mtR,i,j) = t = VP(aT[i],aT[j],H);
	VSA(aT[j],aT[i],-t,W);
      }else{
	Elem(mtR,i,i) = t = sqrt(VP(aT[i],aT[i],H));
	VSS(aT[i],1/t,W);
      }
    }
  }
}

void MatrixSimeqLr(Matrix*mtB,Matrix*mtR){
  // B = B L^{-1}
  double * B = Row(mtB,0);
  int i,j;
  int W = mtR->W - 1;

  for(i=W;i>=0;i--){
    B[i]=B[i];
    for(j=W;i>=0;j--){
      if(i==j)break;
      B[i]=B[i]-B[j]*Elem(mtR,i,j);
    }
    B[i]=B[i]/Elem(mtR,i,i);
  }
}


main(){
  Matrix *cmA, *vt, *mtR, *tmp;
  int i;
  double z=1;

  /*
  double xy[][2]={ // from 0.jpg
    256,218,
    347,220,
    263,367,
    413,315,
  },uv[][2]={ // from 1.jpg
    371,230,
    463,230,
    383,379,
    530,327,
  };
  */

  double xy[][2]={ // from 0.jpg
    111,212,
    169,354,
    454,455,
    451,221,
    176,531,
  },uv[][2]={ // from 1.jpg
    230,227,
    287,365,
    573,467,
    568,231,
    296,541,
  };

  cmA=MatrixAlloc(8,10);
  vt=MatrixAlloc(1,10);

  // create A (col-major)
  for(i=0;i<5;i++){
    cmA->data[cmA->W*0+(i*2  )]=z*xy[i][0];
    cmA->data[cmA->W*1+(i*2  )]=z*xy[i][1];
    cmA->data[cmA->W*2+(i*2  )]=z*z;
    cmA->data[cmA->W*3+(i*2  )]=0;
    cmA->data[cmA->W*4+(i*2  )]=0;
    cmA->data[cmA->W*5+(i*2  )]=0;
    cmA->data[cmA->W*6+(i*2  )]=-xy[i][0]*uv[i][0];
    cmA->data[cmA->W*7+(i*2  )]=-xy[i][1]*uv[i][0];
    cmA->data[cmA->W*0+(i*2+1)]=0;
    cmA->data[cmA->W*1+(i*2+1)]=0;
    cmA->data[cmA->W*2+(i*2+1)]=0;
    cmA->data[cmA->W*3+(i*2+1)]=z*xy[i][0];
    cmA->data[cmA->W*4+(i*2+1)]=z*xy[i][1];
    cmA->data[cmA->W*5+(i*2+1)]=z*z;
    cmA->data[cmA->W*6+(i*2+1)]=-xy[i][0]*uv[i][1];
    cmA->data[cmA->W*7+(i*2+1)]=-xy[i][1]*uv[i][1];
    vt->data[i*2  ]=z*uv[i][0];
    vt->data[i*2+1]=z*uv[i][1];
  }

  // solve Least-squares equation
  mtR=MatrixAlloc(8,8);
  MatrixQRDecompColMajor(mtR,cmA);
  tmp=MatrixAlloc(1,8);
  MatrixMultT(tmp,vt,cmA);
  MatrixSimeqLr(tmp,mtR);
  MatrixPrint(tmp);
}
