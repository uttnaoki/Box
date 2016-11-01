#include<math.h>
#include<string.h>

#define ImageImageProjectionAlpha ImageImageProjectionAlphaNaive



//画像の初期化を行う（黒=0で埋める）（自作）
void ImageClear(Image*im){
  int x,y;
  for(y=0;y<im->H;y++) for(x=0;x<im->W;x++){
      //IElemはimage.hにdefine文で定義されている
      //#define IElem(_im,_x,_y,_c) (_im) -> data[(_y)*(_im)->W*3 +(_x)*3+(_c)]
      IElem(im,x,y,0)=0;
      IElem(im,x,y,1)=0;
      IElem(im,x,y,2)=0;
    }
}



//3*3行列の積を計算する関数
//ans=m1*m2みたいな感じ
void mult33(double ans[][3], double m1[][3], double m2[][3]){
  int i,j,k;
    
  for ( i=0; i< 3; ++i){
    for ( j=0; j< 3; ++j){
      ans[i][j] = 0;//初期化

      for( k=0; k< 3; ++k){
	ans[i][j] += m1[i][k]*m2[k][j];
      }//積の計算
    }
  }
}






//RGB各要素をalpha倍して加算する関数
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











//ここからlsq.cの関数とか



// basic vector operations
//ベクトルの内積の計算
double VP(double*a,double*b,int N){
  double s=0;
  int i;
  for(i=0;i<N;i++) s += a[i] * b[i] ;
  return s;
}

//ベクトルの各要素をs倍（b=sbみたいな）
void VSS(double*d,double s,int N){
  int i;
  for(i=0;i<N;i++) d[i] *= s;
}

//ベクトルbにs倍したaをたす（b=b+saみたいな）
void VSA(double*d,double*a,double s,int N){
  int i;
  for(i=0;i<N;i++) d[i] += a[i] * s;
}


#define Row(_a,_b)     ((_a)->data+(_a)->W*(_b))
//Elemは行列の指定した箇所に値を代入するもの
//Rowは第b行へのポインタを取得する




//行列を0で初期化
void MatrixClear(Matrix*mt){
  memset(mt->data,0,mt->W*mt->H*sizeof(double));
}

//行列mtを行列mtDへコピー
void MatrixCopy(Matrix*mtD,Matrix*mt){
  memmove(mtD->data,mt->data,mt->W*mt->H*sizeof(double));
}


//行ベクトルmt[8]の末尾に1をたして配列[3][3]を返す（自作）
void Vec_attacch1_to_Matrix(double ans[3][3],Matrix*mt){

  int i,j;
  for(i=0;i<3;i++){
    for(j=0;j<3;j++){
      if(i==2 && j==2){ans[i][j]=1;
      }else{ans[i][j] = Elem(mt,0,i*3+j);}
    }
  }//forループここまで
}


//行列mtの転置行列をmtDへコピー
void MatrixCopyT(Matrix*mtD,Matrix*mt){
  int i,j;
  for(i=0;i<mtD->H;i++)
    for(j=0;j<mtD->W;j++)
      Elem(mtD,i,j) = Elem(mt,j,i);
}

//行列を表示する
void MatrixPrint(Matrix*mt){
  int i,j;
  for(i=0;i<mt->H;i++){
    for(j=0;j<mt->W;j++){
      printf("%f ",Elem(mt,i,j));
      if(j%3==2)printf("\n");
    }
    printf("\n");
  }
}

//mtD= 行列mtA × mtBの転置行列？
void MatrixMultT(Matrix*mtD,Matrix*mtA,Matrix*mtB){
  // D = A B^T
  int i,j;
  for(i=0;i<mtA->H;i++)
    for(j=0;j<mtB->H;j++)
      Elem(mtD,i,j) = VP( Row(mtA,i), Row(mtB,j), mtA->W);
  //関数VPはベクトルの内積の計算
  //Elemは行列の指定した箇所に値を代入するもの
}

//行列mtの行ベクトルをQR分解しmtRに格納していく（自分で改変）
//Qは列直交行列、Rは上三角行列のこと
//直交行列の逆行列は転置行列に等しい
void MatrixQRDecompColMajor(Matrix*mtR,Matrix*mt){
  // gram-schmidt orthonormalization (Lt and Q)
  double t, *aT[] = { Row(mt,0), Row(mt,1), Row(mt,2), Row(mt,3), Row(mt,4), Row(mt,5), Row(mt,6), Row(mt,7)} ;//ここを変更することに注意
  int W = mt->W;
  int i,j;
  MatrixClear(mtR);

 


  //このforループで全部済ませられる
  for(i=0;i<8;i++){
    // fprintf(stderr,"i_loop_%d\n",i);
    for(j=0;i>j;j++){
      //fprintf(stderr,"j_loop_%d\n",j);
      Elem(mtR,j,i) = t = VP(aT[j], aT[i], W);
      //fprintf(stderr,"....Elem_end\n");
      VSA(aT[i], aT[j], -t, W);
    }
    //fprintf(stderr,"....j_loop_end\n");
    Elem(mtR,i,i) = t = sqrt(VP(aT[i],aT[i],W));
    VSS(aT[i], 1/t, W);
  }//ここまでforループ



}
//最小2乗法で方程式Rx=bを解く（自分で改変）
void MatrixSimeqLr(Matrix*mtB,Matrix*mtR){
  // B = B L^{-1}
  double * B = Row(mtB,0);
  double siguma=0;
  int i,j;

  //このforループで全部済ませられる
  for(i=7;i>=0;i--){
    for(j=7;j>i;j--){
      siguma+=B[j]*Elem(mtR,i,j);
    }
    B[i]=(B[i]-siguma)/Elem(mtR,i,i);
    siguma=0;
  }//ここまでforループ

}







//配列[3][3]の中身を表示（デバッグ用）
void Print_mt33(double mt[3][3]){
  int i,j;
  for(i=0;i<3;i++){
    for(j=0;j<3;j++){
      printf("%f ",mt[i][j]);
      if(j%3==2)printf("\n");
    }
    //printf("\n");
  }
}


