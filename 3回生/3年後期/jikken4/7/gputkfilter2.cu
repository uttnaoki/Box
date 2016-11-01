#include"image.h"
#define getpix(x,y) img[((x)+imW*(y))*3+1]

__global__ void gpuTK_vertical(float*tmp,unsigned char *img,int imW,int imH){
  int x=blockDim.x*blockIdx.x+threadIdx.x;
  int y=blockDim.y*blockIdx.y+threadIdx.y;
  int v,W=7;
  int mat=imW*imH;

  if(W+1<=y && y<imH-W-1)
    if(1<=x && x<imW-1){
      float ix,iy,ixx,ixy,iyy;
      ixx=iyy=ixy=0;
      for(v=-W;v<=W;v++){
	ix=getpix(x+1,y+v)-getpix(x-1,y+v);
	iy=getpix(x,y+v+1)-getpix(x,y+v-1);
	ixx+=ix*ix;
	ixy+=ix*iy;
	iyy+=iy*iy;
      }
      tmp[(x+imW*y)]=ixx;
      tmp[(x+imW*y+mat)]=ixy;
      tmp[(x+imW*y+mat*2)]=iyy;
    }
}

__global__ void gpuTK_horizontal(double*fimg,float*tmp,int imW,int imH){
  int x=blockDim.x*blockIdx.x+threadIdx.x;
  int y=blockDim.y*blockIdx.y+threadIdx.y;
  int u,W=7;
  int mat=imW*imH;

  if(W+1<=y && y<imH-W-1 &&
     W+1<=x && x<imW-W-1){
      float ixx,ixy,iyy;
      double lamd;
      ixx=iyy=ixy=0;
      for(u=-W;u<=W;u++){
	ixx+=tmp[(x+u+imW*y)];
	ixy+=tmp[(x+u+imW*y+mat)];
	iyy+=tmp[(x+u+imW*y+mat*2)];
      }
      lamd=((ixx+iyy)-sqrt(pow(ixx+iyy,2)-4*(ixx*iyy-ixy*ixy)))/2;
      fimg[x+imW*y]=lamd;
    }else fimg[x+imW*y]=0;
}

// TKfilter.c では ImageFeature 本体を除去して，
// プロトタイプ宣言 void ImageFeature(Matrix*im2,Image*im); のみを書く．

extern "C"
void ImageFeature(Matrix*im2,Image*im){
  double*d_dst;
  float *d_tmp;
  unsigned char*d_src;
  cudaMalloc(&d_src,im->W*im->H*3);
  cudaMalloc(&d_dst,sizeof(double)*im->W*im->H);
  cudaMalloc(&d_tmp,sizeof(float)*im->W*im->H*3);
  cudaMemcpy(d_src,im->data,im->W*im->H*3,cudaMemcpyHostToDevice);
  gpuTK_vertical<<<dim3((im->W+15)/16,(im->H+15)/16),dim3(16,16)>>>(d_tmp,d_src,im->W,im->H);
  gpuTK_horizontal<<<dim3((im->W+15)/16,(im->H+15)/16),dim3(16,16)>>>(d_dst,d_tmp,im->W,im->H);  
  cudaMemcpy(im2->data,d_dst,im->W*im->H*sizeof(double),cudaMemcpyDeviceToHost);
  cudaFree(d_dst);
  cudaFree(d_src);
  cudaFree(d_tmp);
}