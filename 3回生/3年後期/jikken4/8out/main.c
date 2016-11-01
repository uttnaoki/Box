#include "image.h"
#define MAX 30
#define fnum 4

Matrix*MatrixAlloc(int _H,int _W){
  Matrix*mt=(Matrix*)malloc(sizeof(Matrix));;
  mt->W = _W,
  mt->H = _H;
  mt->data=(double*)malloc(mt->W*mt->H*sizeof(double));
  return mt;
}

void RANSAC(int rndAry[MAX]){
  int i;
  for(i=0;i < fnum;i++){
  int j, t;
  j=(int)(drand48()*MAX);
  t=rndAry[i]; rndAry[i]=rndAry[j]; rndAry[j]=t;
  }
}

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
    if((dx*dx+dy*dy)<5) n++;
  }
  return n;
}

void main(int ac,char**av){
  Image *im,*im2,*im3;
  int rndAry[MAX];
  int i,j,k,l;
  double xy[fnum][2],uv[fnum][2];
  int maxf=0;
  int cf;
  double bb[8];
  int w[30][4];
  //FILE *fp=fopen("lsq.txt","w");
  double trance[9];
  Matrix*aa;
  int N=100;
  int x1[100][2],x2[100][2],x3[100][2];

  im = ImageRead(av[1]);
  im2= ImageRead(av[2]);
  if(ac==4)im3=ImageRead(av[3]);

  TK(av[1],x1);
  TK(av[2],x2);

  for(i=0;i<MAX;i++) rndAry[i]=i;
  aa=MatrixAlloc(1,8);
 
  GREEDY(N,N,x1,x2,im,im2,w);

  for(l=0;l<30000;l++){
    RANSAC(rndAry);
    for(i=0;i<fnum;i++){
      xy[i][0]=w[rndAry[i]][0];xy[i][1]=w[rndAry[i]][1];
      uv[i][0]=w[rndAry[i]][2];uv[i][1]=w[rndAry[i]][3];
    }
    LSQ(aa,xy,uv);
    cf=count_feature(aa,w);
    if(maxf<cf){
      maxf=cf;
      for(i=0;i<8;i++)bb[i]=Elem(aa,0,i);
    }
  }
  printf("%d\n",maxf);
  for(i=0;i<8;i++){
    trance[i]=bb[i];
    printf("%f,",bb[i]);
  }
  trance[i]=1;
  /*
  out=ImageAlloc(1024,768);
  ImageClear(out);

  Image *out;
  double m0d[][3]={
      1,0,-100,
      0,1,-100,
      0,0,1
    };
  //im=ImageRead("0.jpg");
    ImageImageProjectionAlpha(out,im,m0d,.5);
  */


  PANO(im,im2,"test.jpg",trance);
  /*
  //Image *imout=ImageRead("test.jpg");
  TK(av[3],x3);
  for(i=0;i<MAX;i++) rndAry[i]=i;
  GREEDY(N,N,x1,x3,im,im3,w);
  for(l=0;l<30000;l++){
    RANSAC(rndAry);
    for(i=0;i<fnum;i++){
      xy[i][0]=w[rndAry[i]][0];xy[i][1]=w[rndAry[i]][1];
      uv[i][0]=w[rndAry[i]][2];uv[i][1]=w[rndAry[i]][3];
    }
    LSQ(aa,xy,uv);
    cf=count_feature(aa,w);
    if(maxf<cf){
      maxf=cf;
      for(i=0;i<8;i++)bb[i]=Elem(aa,0,i);
    }
  }
  printf("%d\n",maxf);
  for(i=0;i<8;i++)trance[i]=bb[i];
  trance[i]=1;
  PANO(imout,im3,"test2.jpg",trance);
  */


}
/*
gcc -O main.c TKfilter2.c pano0.c greedy.c image.c lsq.c -lm;
./a.out 0.jpg 2.jpg;

*/
