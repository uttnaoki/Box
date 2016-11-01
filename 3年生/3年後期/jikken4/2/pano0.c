// gcc -O pano0.c image.c && ./a.out && display out.ppm

#include"image.h"
#include <string.h>

// Following functions must be defined in image.c:
//   ImageRead, ImageAlloc, ImageClear.
// In addition, check if your ImageRead can read a jpg file.

#define ImageImageProjectionAlpha ImageImageProjectionAlphaNaive

void ImageImageProjectionAlpha(Image*id,Image*is,double a[3][3],double alpha){
  int x,y,u,v;
  double r;
  for(y=0;y<id->H;y++) for(x=0;x<id->W;x++){
    r = 1 / (a[2][0]*x+a[2][1]*y+a[2][2]);
    u = r * (a[0][0]*x+a[0][1]*y+a[0][2]);
    v = r * (a[1][0]*x+a[1][1]*y+a[1][2]);
    if( isInsideImage(is,u,v) ){
      id->data[(y*id->W+x)*3+0] += is->data[(v*is->W+u)*3+0]*alpha,
      id->data[(y*id->W+x)*3+1] += is->data[(v*is->W+u)*3+1]*alpha,
      id->data[(y*id->W+x)*3+2] += is->data[(v*is->W+u)*3+2]*alpha;
    }
  }
}

void mult33(double a[3][3],double b[3][3], double c[3][3]){
  int i,j,k;
  for(i=0;i<3;i++) {
    for(j=0;j<3;j++) {
      for(k=0;k<3;k++) {
	a[i][j]+=b[i][k]*c[k][j];
      }
    }
  }
}

int main(int argc,char *argv[]){
  Image *im,*im2;
  int i,j;

  im2=ImageAlloc(1024,768);
  ImageClear(im2);
  
  double m0d[][3]={
      1,0,-100,
      0,1,-100,
      0,0,1
    };
    im=ImageRead("0.jpg");
    ImageImageProjectionAlpha(im2,im,m0d,.5);
  /*
  {
    double m1d[][3]={
       0.980063, 0.155844, -15.090362,
      -0.055756, 1.153389, -109.259360,
      -0.000139, 0.000316, 0.982279
    };
    im=ImageRead("1.jpg");
    ImageImageProjectionAlpha(im2,im,m1d,.5);
    }
  */
  
    double m10[][3]={
      0.980063, 0.155844, 98.500361,
      -0.055756, 1.153389, 0.503900,
      -0.000139, 0.000316, 1
    }, m1d[3][3];
    memset(m1d,sizeof(m1d),0);
    mult33(m1d,m10,m0d);
    for(i=0;i<3;i++){
      for(j=0;j<3;j++) printf("%f ",m1d[i][j]);
      printf("\n");
    }
    im=ImageRead("1.jpg");
    ImageImageProjectionAlpha(im2,im,m1d,.5);
  

  ImageWrite(argv[1],im2);

  return 0;
}
