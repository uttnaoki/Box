// gcc -O easyprocess.c
// cl /Ox easyprocess.c

#include"image.h"

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
  sprintf(cmd,"djpeg %s",name);
  FILE*fp=popen(cmd,"r");
  fscanf(fp,"%*s%d%d%*s%*c",&W,&H);
  im=ImageAlloc(W,H);
  fread(im->data,1,W*H*3,fp);
  pclose(fp);
  return im;
}

void ImageWrite(const char*name,Image*im){
  char cmd[256];
  sprintf(cmd,"cjpeg>%s",name);
  FILE*fp=popen(cmd,"w");
  fprintf(fp,"P6 %d %d 255\n",im -> W,im -> H);
  fwrite(im -> data,1,im -> W*im ->H*3,fp);
  pclose(fp);
}

void Process(Image*dst,Image*src,int flag){
  int x,y;
  for(y=0;y<src->H;y++)
    for(x=0;x<src->W;x++){
      /*int rr=y<src->H*2/3;
      int gg=x<src->W*2/3 && src->H/3<y;
      int bb=src->W/3<x && src->H/3<y;
      IElem(dst,x,y,0)=rr ? IElem(src,x,y,0) : 0;
      IElem(dst,x,y,1)=gg ? IElem(src,x,y,1) : 0;
      IElem(dst,x,y,2)=bb ? IElem(src,x,y,2) : 0;*/
      if(flag==0){
	IElem(dst,x,y,0)=IElem(src,x,y,0);
	IElem(dst,x,y,1)=0;
	IElem(dst,x,y,2)=0;
      }
      if(flag==1){
	IElem(dst,x,y,0)=0;
	IElem(dst,x,y,1)=IElem(src,x,y,1);
	IElem(dst,x,y,2)=0;
      }
      if(flag==2){
	IElem(dst,x,y,0)=0;
	IElem(dst,x,y,1)=0;
	IElem(dst,x,y,2)=IElem(src,x,y,2);
      }
    }
}

main(int argc,char**argv){
  //  Image *dst;
  Image *src;
  Image *dstr,*dstg,*dstb;
  char R[20],G[20],B[20];

  sprintf(R,"%sR.jpg",argv[2]);
  sprintf(G,"%sG.jpg",argv[2]);
  sprintf(B,"%sB.jpg",argv[2]);

  src=ImageRead(argv[1]);
  dstr=ImageAlloc(src->W,src->H);
  dstg=ImageAlloc(src->W,src->H);
  dstb=ImageAlloc(src->W,src->H);
  Process(dstr,src,0);
  Process(dstg,src,1);
  Process(dstb,src,2);
  ImageWrite(R,dstr);
  ImageWrite(G,dstg);
  ImageWrite(B,dstb);
}
