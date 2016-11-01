#include<stdio.h>
#include<stdlib.h>

main(){
  int width=640, height=480;
  unsigned char*img=(unsigned char*)malloc(height*width*3);

  {
#define DIST2(x,y,xx,yy) (x-xx)*(x-xx)+(y-yy)*(y-yy)
    int y,x;
    for(y=0;y<height;y++)
      for(x=0;x<width;x++){
        img[(y*width+x)*3+0]=DIST2(x,y,320,180)<65536 ? 255 : 0; // 座標(x,y)の赤輝度
	img[(y*width+x)*3+1]=DIST2(x,y,260,300)<65536 ? 255 : 0; // 座標(x,y)の緑輝度
	img[(y*width+x)*3+2]=DIST2(x,y,380,300)<65536 ? 255 : 0; // 座標(x,y)の青輝度
      }
  }

  printf("P6 %d %d 255\n",width,height);
  fwrite(img,1,width*height*3,stdout);
}
