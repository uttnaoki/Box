#include <stdlib.h> //qsort用


//#define INFINITY (1/0.)


typedef struct{
  int strc_i;
  int strc_j;
  double ssd;
}Array_min;

Array_min tmp[(N1_max)*(N2_max)];


//画像1におけるi番目の特徴点と画像2におけるj番目の特徴点の、SSD(i,j)を計算する
double ImageSSD(Image*im,int x1,int y1, Image*im2,int x2,int y2){
  //画像1から(x1,y1)、画像2から(x2,y2)
  int i,j,W=7;//特徴点を中心とした15*15の矩形領域を用いる
  double sr=0,sg=0,sb=0,dr,dg,db;
  for(i=-W;i<=W;i++) for(j=-W;j<=W;j++){
      dr  = IElem(im, x1+j, y1+i, 0) - IElem(im2, x2+j , y2+i, 0);//R成分の差(以下、「差R」と言う)
      dg  = IElem(im, x1+j, y1+i, 1) - IElem(im2, x2+j , y2+i, 1);//G成分の差(以下、「差G」と言う)
      db  = IElem(im, x1+j, y1+i, 2) - IElem(im2, x2+j , y2+i, 2);//B成分の差(以下、「差B」と言う)
      sr += dr*dr;//差Rの２乗の総和
      sg += dg*dg;//差Gの２乗の総和
      sb += db*db;//差Bの２乗の総和
    }
  return sr+sg+sb;
}

//SSDの表を行列mtとして作成し、構造体にも格納
void calcSSDtable(Matrix*mt,
		  Image*im ,int x1[][2],int N1,
		  Image*im2,int x2[][2],int N2){
  int i,j;
  for(i=0;i<N1;i++){
    for(j=0;j<N2;j++){
      //行列mtの(i,j)にSSD(i,j)を格納
      Elem(mt,i,j)=ImageSSD(im ,x1[i][0],x1[i][1],im2,x2[j][0],x2[j][1]);
    }
  }
}

//行列から構造体へコピー
void mt_to_strc(Matrix *mt,int N1,int N2){
  int i,j,k=0;

  for(i=0;i<N1;i++){
    for(j=0;j<N2;j++){
      tmp[k].strc_i=i;
      tmp[k].strc_j=j;
      tmp[k].ssd=Elem(mt,i,j);
      k++;
    }
  }
}


//比較関数 qsort用
int comp_ssd( const void *c1, const void *c2 )
{
  Array_min test1 = *(Array_min *)c1;
  Array_min test2 = *(Array_min *)c2;

  double tmp1 = test1.ssd;   /* ssd を基準とする */
  double tmp2 = test2.ssd;

  return (int)(tmp1 - tmp2); //小さい順にソート
}


//SSDの表mtから特徴点の対応づけをする
//方法１
//表のi行目においてSSDを最小とするところ(列番号)を記録する。
//imやim2どこで使う？
int matchMethod1(int match[][2],Matrix*mt,
		 Image*im ,int x1[][2],int N1,
		 Image*im2,int x2[][2],int N2){
  int i,j,k,ji=0,n=0;
  //-Wallでコンパイルしたときにエラーがでるのでjiを初期化したが、必ずji=jとなるので、しなくてもよい
  for(i=0;i<N1;i++){
    double sm=INFINITY,t;
    for(j=0;j<N2;j++){
      t=Elem(mt,i,j);//tはmtの(i,j)番目
      // printf("%f ",t);
      if( sm > t ) sm = t, ji=j;//SSDが最小となるときのjを更新していく
    }//jやN2のforここまで
    match[n][0]=i; match[n][1]=ji; n++;
    
    /*    
	  printf("%d,%d,%d,%d,\n",
	  x1[i][0],x1[i][1],
	  x2[ji][0],x2[ji][1]);
    */

    for(k=0;k<N1;k++) Elem(mt,k,ji) = INFINITY;
    //一度記録した点は無限大にして、今後使われないようにする
  }//iやN1のforここまで

  return n;//N1になるはず
}



//SSDの表mtから特徴点の対応づけをする
//方法2？
//SSDの小さい順に対応点を探す
//本来なら行列から行や列を削除した方がいいが、うまくできないので無限大にしている
int matchMethod2(int match[][2],Matrix*mt,
		 Image*im ,int x1[][2],int N1,
		 Image*im2,int x2[][2],int N2){
  int i, j, k=0;
   //int ct=0;
  // int infi=0;
  for(k=0;(k<N1||k<N2);k++){
    mt_to_strc(mt,N1,N2);
  
    qsort(tmp,(N1*N2),sizeof(Array_min),comp_ssd);
   
    //tmp[0]が最小になるようソート
    if(tmp[0].ssd==INFINITY)return k;
    match[k][0]=tmp[0].strc_i;
    match[k][1]=tmp[0].strc_j;
    // printf("ssd= %f\n",tmp[0].ssd);//デバッグ用
    for(i=0;i<N1;i++) Elem(mt,i,tmp[0].strc_j)= INFINITY;
    for(j=0;j<N2;j++) Elem(mt,tmp[0].strc_i,j)= INFINITY;
  }  
 
  return k;//N1かN2になるはず?
}
