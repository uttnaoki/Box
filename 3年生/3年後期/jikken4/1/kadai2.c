//jpg -> ppm

#include "image.h"
Image*ImageAlloc(int W,int H){
  Image*im=(Image*)malloc(sizeof(Image));
  im->W=W;
  im->H=H;
  im->data=(unsigned char*)malloc(W*H*3);
  return im;
}

Image*ImageRead(const char*name){
  int W,H;
  Image*im;
  char cmd[256];
  FILE*fp;
  sprintf(cmd,"djpeg %s",name);
  fp=popen(cmd,"r");
  fscanf(fp,"%*s%d%d%*s%*c",&W,&H);
  im=ImageAlloc(W,H);
  fread(im->data,1,W*H*3,fp);
  pclose(fp);
  return im;
}

void ImageWrite(const char*name,Image*im){
  char cmd[256];
  FILE *fp=fopen(name,"wb");
  //sprintf(cmd,"djpeg > %s",name);
  //fp=popen(cmd,"w");
  fprintf(fp,"P6 %d %d 255\n",im->W,im->H);
  fwrite(im->data,1,im->W*im->H*3,fp);
  fclose(fp);
}

void Process(Image*dst,Image*src){
  int x,y;
  for(y=0;y<src->H;y++)
    for(x=0;x<src->W;x++){
      int rr=y<src->H*2/3;
      int gg=x<src->W*2/3 && src->H/3<y;
      int bb=src->W/3<x && src->H/3<y;
      IElem(dst,x,y,0)=rr ? IElem(src,x,y,0) : 0;
      IElem(dst,x,y,1)=gg ? IElem(src,x,y,1) : 0;
      IElem(dst,x,y,2)=bb ? IElem(src,x,y,2) : 0;
    }
}

main(int argc,char**argv){
  Image *dst,*src;

  src=ImageRead(argv[1]);
  dst=ImageAlloc(src->W,src->H);
  Process(dst,src);
  ImageWrite(argv[2],dst);
}
