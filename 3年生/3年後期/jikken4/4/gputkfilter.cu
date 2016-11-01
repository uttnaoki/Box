__global__ void gpuTK(double*fimg,unsigned char*img,int imW,int imH){
#define getpix(x,y) img[((x)+imW*(y))*3+1]
  int x=blockDim.x*blockIdx.x+threadIdx.x;
  int y=blockDim.y*blockIdx.y+threadIdx.y;
  int u,v,W=7;

  if(W+1<=y && y<imH-W-1){
    if(W+1<=x && x<imW-W-1){
      float ix,iy,ixx,ixy,iyy,lamd;
      ixx=iyy=ixy=0;
      for(v=-W;v<=W;v++)
	for(u=-W;u<=W;u++){
	  ix=getpix(x+u+1,y+v)-getpix(x+u-1,y+v);
	  iy=getpix(x+u,y+v+1)-getpix(x+u,y+v-1);
	  ixx+=ix*ix; // <-- incomplete
	  ixy+=ix*iy;
	  iyy+=iy*iy;
	}
	lamd=((ixx+iyy)-sqrt(pow(ixx+iyy,2)-4*(ixx*iyy-pow(ixy,2))))/2;
      fimg[x+imW*y]=lamd; // <-- incomplete
    }
  }else fimg[x+imW*y]=0;
}

#include"image.h"

typedef struct {
  double *data;
  int W,H;
} Matrix;

// TKfilter.c では ImageFeature 本体を除去して，
// プロトタイプ宣言 void ImageFeature(Matrix*im2,Image*im); のみを書く．

extern "C"
void ImageFeature(Matrix*im2,Image*im){
  double*d_dst;
  unsigned char*d_src;
  cudaMalloc(&d_src,im->W*im->H*3);
  cudaMalloc(&d_dst,sizeof(double)*im->W*im->H);
  cudaMemcpy(d_src,im->data,im->W*im->H*3,cudaMemcpyHostToDevice);
  gpuTK<<<dim3((im->W+15)/16,(im->H+15)/16),dim3(16,16)>>>(d_dst,d_src,im->W,im->H);
  cudaMemcpy(im2->data,d_dst,im->W*im->H*sizeof(double),cudaMemcpyDeviceToHost);
  cudaFree(d_dst);
  cudaFree(d_src);
}