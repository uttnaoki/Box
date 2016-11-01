//gcc に必ず -lm つける

#define N1_max 30
#define N2_max 30
#define N_max 30//Tkfilter用

#include"my_image.h"  //画像の読み書き
#include"my_pano.h"   //行列の計算
#include"my_tkfilter.h"  //特徴点の計算
#include"my_greedy2.h"  //特徴点の対応づけSSDの計算とか


#define MAX 100


//グローバル関数としてすでに使われているもの
/*
  Array_min tmp[(N1_max)*(N2_max)];

*/

//ヘッダファイルで定義されているマクロ
/*

  #define IElem(_im,_x,_y,_c) (_im) -> data[(_y)*(_im)->W*3 +(_x)*3+(_c)]
  #define isInsideImage(is,u,v) (((unsigned)u<is->W)&&((unsigned)v<is->H))
  #define DElem(_a,_c,_b)  (_a)->data[(_a)->W*(_b)+(_c)]//自作
  #define Elem(_a,_b,_c)  (_a)->data[(_a)->W*(_b)+(_c)]
  #define INFINITY (1/0.)
*/



//主にTKfilter.c関連
//特徴点の抽出
void FeatureExtraction(Image *im, int w[][2]){

  Matrix*mt_fea;
  mt_fea=MatrixAlloc(im->H,im->W);
  ImageFeature(mt_fea,im);//imの特徴点の結果をmt_feaに格納
  MatrixLocalMax_qsort2(w,mt_fea);//↑のmt_feaを使って計算
}




//数学のnCrを行う関数
double combi(int n, int r){
  if(n==r){ 
    return 1;
  }else if(r==0){ 
    return 1;
  }else if(r==1){ 
    return n;
  }else{
    return combi(n-1, r-1)+combi(n-1, r);//nCr=n-1Cr-1 + n-1Crを利用
  }
}


//3*3配列のコピー m1=m2みたいな
void copy33(double m1[][3], double m2[][3]){
  int i,j;
    
  for ( i=0; i< 3; i++){
    for ( j=0; j< 3; j++){
      m1[i][j]=m2[i][j];
    }
  }


}




void calc_my_m10(double my_m10[3][3],int xy[][2],int uv[][2],double z){

  int i;
  Matrix *cmA, *vt, *mtR, *mt_tmp;

  cmA=MatrixAlloc(8,8);
  vt=MatrixAlloc(1,8);


  // create A (col-major)
  //行列xyとuvから行列Aを求める。行列vt（pdfだとb）も計算する
  for(i=0;i<4;i++){
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
   

  //   MatrixPrint(cmA);printf("\n");
  mtR=MatrixAlloc(8,8);
  //fprintf(stderr,"QR in\n\n");
  MatrixQRDecompColMajor(mtR,cmA);  //QR変換が行われ、mtRにはもとの行列cmAのRが、cmAはQになっている
  //fprintf(stderr,"QR out\n");
  mt_tmp=MatrixAlloc(1,8);
  MatrixMultT(mt_tmp,vt,cmA);//mt_tmp=vt*cmA
  MatrixSimeqLr(mt_tmp,mtR);
  //方程式mtR*x=mt_tmpを解く、解であるxはmt_tmpへ上書きされる．
  //    MatrixPrint(mt_tmp);//解を表示
  //printf("\n");
  
  //mt_tmpに1をつけたして3×3行列へ
  Vec_attacch1_to_Matrix(my_m10,mt_tmp);
  // if(n%1000==0) Print_mt33(my_m10);//デバッグ用my_m10を表示
}










//主にpano0.c関連とgreedy.c関連とransac関連
//2枚の画像の対応づけ
void Match(Image *im1, Image *im2,int N1, int N2, int x1[][2], int x2[][2],double m0d[3][3],double m1d[3][3]){
  
  Matrix *mt;
  int nm;
  int match[999][2];
  int rndAry[MAX];
  
  double z=1;
  double xd, yd, zd, ud, vd;

  double my_m10[3][3]; 
  double best_m10[3][3];

  int ct=0;
  int ct_tmp=-1;

  int k,n,m,l;//ループ用
  int j,t;//乱数用
   
  int a,b,c,d;
  int a1,a2,b1,b2,c1,c2,d1,d2;
  int tmp_a,tmp_b,tmp_c,tmp_d;


  mt=MatrixAlloc(N1,N2);
  calcSSDtable(mt,im1,x1,N1,im2,x2,N2);

  nm=matchMethod2(match,mt,im1,x1,N1,im2,x2,N2);
   //nm=matchMethod1(match,mt,im1,x1,N1,im2,x2,N2);

  
  for(k=0;k < MAX;k++) rndAry[k]=k;//乱数初期化

  // for(n=0;n<combi(nm,4);n++){//for ransac
  for(n=0;n<combi(nm,4)&&n<100;n++){
     

      for(m=0; m< 4;m++){//for乱数の再設定
	j=(int)(drand48()*nm);	
	t=rndAry[m]; rndAry[m]=rndAry[j]; rndAry[j]=t;
      }//for乱数の再設定ここまで		 
    
      a=rndAry[0];
      b=rndAry[1];
      c=rndAry[2];
      d=rndAry[3];
 	
     a1= match[a][0];
    b1= match[b][0];
    c1= match[c][0];
    d1= match[d][0];
    a2= match[a][1];
    b2= match[b][1];
    c2= match[c][1];
    d2= match[d][1];

 	
    int xy[][2]={ // from the first image
      x1[a1][0], x1[a2][1],
      x1[b1][0], x1[b2][1],
      x1[c1][0], x1[c2][1],
      x1[d1][0], x1[d2][1],
    };
    int uv[][2]={ // from the second image
      x2[a2][0], x2[a2][1],
      x2[b2][0], x2[b2][1],
      x2[c2][0], x2[c2][1],
      x2[d2][0], x2[d2][1],
    };

      calc_my_m10(my_m10,xy,uv,z);

      //対応点の評価のfor
      for(l=0;l<30;l++){
	double x=x1[l][0], y=x1[l][1],
	  u=x2[l][0], v=x2[l][1],
	  dx,dy;
	// dx, dy は (x,y) を変換した座標と (u,v) の差．
     
	xd=my_m10[0][0]*x+my_m10[0][1]*y+my_m10[0][2]*z;
	yd=my_m10[1][0]*x+my_m10[1][1]*y+my_m10[1][2]*z;
	zd=my_m10[2][0]*x+my_m10[2][1]*y+my_m10[2][2]*z;

	ud=xd/zd;
	vd=yd/zd;
    
	dx=u-ud;
	dy=v-vd;
     
    
	if(dx*dx+dy*dy<10) ct++;
	//}//対応点の評価のforここまで
    
	if(ct>ct_tmp){
	  ct_tmp=ct;
	  //   printf("\n\nloop_n=%d\n",n);
	  //Print_mt33(my_m10);//デバッグ用my_m10を表示
	

	  //copy33(best_m10,my_m10);
	  tmp_a=a;
	  tmp_b=b;
	  tmp_c=c;
	  tmp_d=d;
	  // printf("\n");
	  /// Print_mt33(best_m10);//デバッグ用my_m10を表示
	}
	 ct=0;
      }//対応点の評価のforここまで


    }//for ransacここまで

    a1= match[tmp_a][0];
    b1= match[tmp_b][0];
    c1= match[tmp_c][0];
    d1= match[tmp_d][0];
    a2= match[tmp_a][1];
    b2= match[tmp_b][1];
    c2= match[tmp_c][1];
    d2= match[tmp_d][1];


    

  int tmp_xy[][2]={ // from the first image
    x1[a1][0], x1[a1][1],
    x1[b1][0], x1[b1][1],
    x1[c1][0], x1[c1][1],
    x1[d1][0], x1[d1][1],
  };
  int tmp_uv[][2]={ // from the second image
    x2[a2][0], x2[a2][1],
    x2[b2][0], x2[b2][1],
    x2[c2][0], x2[c2][1],
    x2[d2][0], x2[d2][1],
  };
 
  /*
  printf("(x,y)=\n(%d, %d),\n (%d, %d),\n (%d, %d), \n(%d, %d),\n",x1[a1][0], x1[a1][1], x1[b1][0], x1[b1][1],x1[c1][0], x1[c1][1],x1[d1][0], x1[d1][1]);
  printf("(u,v)=\n(%d, %d),\n (%d, %d),\n (%d, %d),\n (%d, %d),\n\n",x2[a2][0], x2[a2][1], x2[b2][0], x2[b2][1],x2[c2][0], x2[c2][1],x2[d2][0], x2[d2][1]);
  */

  printf("x,y,u,v,\n");
  printf("%d,%d,%d,%d\n",x1[a1][0],x1[a1][1],x2[a2][0],x2[a2][1]);
  printf("%d,%d,%d,%d\n",x1[b1][0],x1[b1][1],x2[b2][0],x2[b2][1]);
  printf("%d,%d,%d,%d\n",x1[c1][0],x1[c1][1],x2[c2][0],x2[c2][1]);
  printf("%d,%d,%d,%d\n",x1[d1][0],x1[d1][1],x2[d2][0],x2[d2][1]);





  calc_my_m10(best_m10,tmp_xy,tmp_uv,z);

  mult33(m1d,best_m10,m0d);

}









//まとめると
int main(int ac, char**argv){
  
  int N1=N1_max;
  int N2=N2_max;
  Image  *imout, *im1, *im2;
  int x1[N1][2], x2[N2][2];
  double m0d[3][3]={
    1,0,-100,
    0,1,-100,
    0,0,1
  };
  double m1d[3][3];

  //3枚以上合成の時は一部変更
  if(ac<3) {
    printf("Not enough input\n ");
    return 1;//入力が少ないときは終了
  }

  //1.新規作成する画像の領域の確保
  imout=ImageAlloc(1024,768);

  //2.画像1の読み取り
  im1=ImageRead(argv[1]);

  //3.画像1の特徴点抽出
  FeatureExtraction(im1,x1);

  //4.画像1を変形して出力画像へ投影
  ImageImageProjectionAlpha(imout,im1,m0d,0.5);

  //5.画像2の読み取り
  im2=ImageRead(argv[2]);
  
  //6.画像2の特徴点抽出
  FeatureExtraction(im2,x2);

  //7.画像1と画像2の対応づけ（m1dを求める）
  Match(im1,im2,N1,N2,x1,x2,m0d,m1d);
 
  //8.画像2を変形して出力画像へ投影
  ImageImageProjectionAlpha(imout,im2,m1d,0.5);

  //9.合成画像の出力
  ImageWrite("matome_2",imout);

  return 0;
}
