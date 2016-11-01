#include"greedy.h"
#include"image.h"
#include"pano.h"
#include"TKfilter.h"

void main(int ac,char**av){
  char outfilename[20];
  Image  *im,*im2,*im3;
  Matrix*mt,*mt2;
  int i,j,k,l,rndAry[MAX];
  double xy[4][2],uv[4][2];
  int maxf=0;
  int cf;
  double bb[9];
  Matrix*aa,*aa2;
  int w[30][4];
  int count=0;
  Image *out;
  double m10[3][3],m1d[3][3];
  Matrix*tk;
  int kk[9999][2],kw;
  aa=MatrixAlloc(1,8);
  aa2=MatrixAlloc(1,8);
  for(i=0;i<MAX;i++) rndAry[i]=i;

  int x1[100][2], N1=100;
  int x2[100][2], N2=100;
  int x3[100][2], N3=100;
  
  printf("Output Filename : ");scanf("%s",outfilename);
  if(strstr(outfilename,"exit")!=NULL)exit(0);

  im =ImageRead(av[1]);
  im2=ImageRead(av[2]);
  im3=ImageRead(av[3]);

  //TKfilter
  tk=MatrixAlloc(im->H,im->W);
  ImageFeature(tk,im);
  kw=MatrixLocalMax(kk,tk);
  for(i=0;i<100;i++){
    x1[i][0]=kk[i][0];
    x1[i][1]=kk[i][1];
  }

  tk=MatrixAlloc(im->H,im->W);
  ImageFeature(tk,im2);
  kw=MatrixLocalMax(kk,tk);
  for(i=0;i<100;i++){
    x2[i][0]=kk[i][0];
    x2[i][1]=kk[i][1];
  }
  
  tk=MatrixAlloc(im->H,im->W);
  ImageFeature(tk,im3);
  kw=MatrixLocalMax(kk,tk);
  for(i=0;i<100;i++){
    x3[i][0]=kk[i][0];
    x3[i][1]=kk[i][1];
  }

  //greedy,lsq
  mt=MatrixAlloc(N1,N2);
  calcSSDtable(mt,im,x1,N1,im2,x2,N2);

  {
    int nm,match[999][2];
    nm=matchMethod(w,mt,im,x1,N1,im2,x2,N2); // 特徴点の対応付け
  }
  for(l=0;l<30000;l++){
    RANSAC(rndAry);
    for(i=0;i<4;i++){
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
  bb[8]=1.0;

  //pano
  out=ImageAlloc(1024,768);
  ImageClear(out);
  double m0d[][3]={
    1,0,-100,
    0,1,-100,
    0,0,1
  };
  ImageImageProjectionAlpha(out,im,m0d,.33);
  for(i=0;i<3;i++)for(j=0;j<3;j++)m10[i][j]=bb[i*3+j];
  memset(m1d,sizeof(m1d),0);
  mult33(m1d,m0d,m10);
  ImageImageProjectionAlpha(out,im2,m1d,.33);
  //ImageWrite(outfilename,out);

  for(i=0;i<9;i++)bb[i]=0;
  memset(aa,sizeof(aa),0);
  memset(w,sizeof(w),0);
  maxf=0;

  //3枚目
  //greedy,lsq
  mt2=MatrixAlloc(N1,N3);
  calcSSDtable(mt2,im,x1,N1,im3,x3,N3);

  {
    int nm,match[999][2];
    nm=matchMethod(w,mt2,im,x1,N1,im3,x3,N3); // 特徴点の対応付け
  }
  for(l=0;l<30000;l++){
    RANSAC(rndAry);
    for(i=0;i<4;i++){
      xy[i][0]=w[rndAry[i]][0];xy[i][1]=w[rndAry[i]][1];
      uv[i][0]=w[rndAry[i]][2];uv[i][1]=w[rndAry[i]][3];
    }
    LSQ(aa2,xy,uv);
    cf=count_feature(aa2,w);
    if(maxf<cf){
      maxf=cf;
      for(i=0;i<8;i++)bb[i]=Elem(aa2,0,i);
    }
  }
  printf("%d\n",maxf);
  bb[8]=1.0;
  //pano
  for(i=0;i<3;i++)for(j=0;j<3;j++)m10[i][j]=bb[i*3+j];
  for(i=0;i<3;i++)for(j=0;j<3;j++)m1d[i][j]=0.0;
  mult33(m1d,m10,m0d);

  ImageImageProjectionAlpha(out,im3,m1d,.33);
  
  ImageWrite(outfilename,out);
}
