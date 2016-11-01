#include <stdlib.h> //qsort�p


//#define INFINITY (1/0.)


typedef struct{
  int strc_i;
  int strc_j;
  double ssd;
}Array_min;

Array_min tmp[(N1_max)*(N2_max)];


//�摜1�ɂ�����i�Ԗڂ̓����_�Ɖ摜2�ɂ�����j�Ԗڂ̓����_�́ASSD(i,j)���v�Z����
double ImageSSD(Image*im,int x1,int y1, Image*im2,int x2,int y2){
  //�摜1����(x1,y1)�A�摜2����(x2,y2)
  int i,j,W=7;//�����_�𒆐S�Ƃ���15*15�̋�`�̈��p����
  double sr=0,sg=0,sb=0,dr,dg,db;
  for(i=-W;i<=W;i++) for(j=-W;j<=W;j++){
      dr  = IElem(im, x1+j, y1+i, 0) - IElem(im2, x2+j , y2+i, 0);//R�����̍�(�ȉ��A�u��R�v�ƌ���)
      dg  = IElem(im, x1+j, y1+i, 1) - IElem(im2, x2+j , y2+i, 1);//G�����̍�(�ȉ��A�u��G�v�ƌ���)
      db  = IElem(im, x1+j, y1+i, 2) - IElem(im2, x2+j , y2+i, 2);//B�����̍�(�ȉ��A�u��B�v�ƌ���)
      sr += dr*dr;//��R�̂Q��̑��a
      sg += dg*dg;//��G�̂Q��̑��a
      sb += db*db;//��B�̂Q��̑��a
    }
  return sr+sg+sb;
}

//SSD�̕\���s��mt�Ƃ��č쐬���A�\���̂ɂ��i�[
void calcSSDtable(Matrix*mt,
		  Image*im ,int x1[][2],int N1,
		  Image*im2,int x2[][2],int N2){
  int i,j;
  for(i=0;i<N1;i++){
    for(j=0;j<N2;j++){
      //�s��mt��(i,j)��SSD(i,j)���i�[
      Elem(mt,i,j)=ImageSSD(im ,x1[i][0],x1[i][1],im2,x2[j][0],x2[j][1]);
    }
  }
}

//�s�񂩂�\���̂փR�s�[
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


//��r�֐� qsort�p
int comp_ssd( const void *c1, const void *c2 )
{
  Array_min test1 = *(Array_min *)c1;
  Array_min test2 = *(Array_min *)c2;

  double tmp1 = test1.ssd;   /* ssd ����Ƃ��� */
  double tmp2 = test2.ssd;

  return (int)(tmp1 - tmp2); //���������Ƀ\�[�g
}


//SSD�̕\mt��������_�̑Ή��Â�������
//���@�P
//�\��i�s�ڂɂ�����SSD���ŏ��Ƃ���Ƃ���(��ԍ�)���L�^����B
//im��im2�ǂ��Ŏg���H
int matchMethod1(int match[][2],Matrix*mt,
		 Image*im ,int x1[][2],int N1,
		 Image*im2,int x2[][2],int N2){
  int i,j,k,ji=0,n=0;
  //-Wall�ŃR���p�C�������Ƃ��ɃG���[���ł�̂�ji���������������A�K��ji=j�ƂȂ�̂ŁA���Ȃ��Ă��悢
  for(i=0;i<N1;i++){
    double sm=INFINITY,t;
    for(j=0;j<N2;j++){
      t=Elem(mt,i,j);//t��mt��(i,j)�Ԗ�
      // printf("%f ",t);
      if( sm > t ) sm = t, ji=j;//SSD���ŏ��ƂȂ�Ƃ���j���X�V���Ă���
    }//j��N2��for�����܂�
    match[n][0]=i; match[n][1]=ji; n++;
    
    /*    
	  printf("%d,%d,%d,%d,\n",
	  x1[i][0],x1[i][1],
	  x2[ji][0],x2[ji][1]);
    */

    for(k=0;k<N1;k++) Elem(mt,k,ji) = INFINITY;
    //��x�L�^�����_�͖�����ɂ��āA����g���Ȃ��悤�ɂ���
  }//i��N1��for�����܂�

  return n;//N1�ɂȂ�͂�
}



//SSD�̕\mt��������_�̑Ή��Â�������
//���@2�H
//SSD�̏��������ɑΉ��_��T��
//�{���Ȃ�s�񂩂�s�����폜���������������A���܂��ł��Ȃ��̂Ŗ�����ɂ��Ă���
int matchMethod2(int match[][2],Matrix*mt,
		 Image*im ,int x1[][2],int N1,
		 Image*im2,int x2[][2],int N2){
  int i, j, k=0;
   //int ct=0;
  // int infi=0;
  for(k=0;(k<N1||k<N2);k++){
    mt_to_strc(mt,N1,N2);
  
    qsort(tmp,(N1*N2),sizeof(Array_min),comp_ssd);
   
    //tmp[0]���ŏ��ɂȂ�悤�\�[�g
    if(tmp[0].ssd==INFINITY)return k;
    match[k][0]=tmp[0].strc_i;
    match[k][1]=tmp[0].strc_j;
    // printf("ssd= %f\n",tmp[0].ssd);//�f�o�b�O�p
    for(i=0;i<N1;i++) Elem(mt,i,tmp[0].strc_j)= INFINITY;
    for(j=0;j<N2;j++) Elem(mt,tmp[0].strc_i,j)= INFINITY;
  }  
 
  return k;//N1��N2�ɂȂ�͂�?
}
