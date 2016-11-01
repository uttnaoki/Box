/***********************************************************
	fft.c --  FFT (���� �t Fourier�ϊ�)

C����ɂ��ŐV�A���S���Y�����T:�������F��:�Z�p�]�_�Ђ��

�ύX:1996.11.7 by M.Doi
�ύX:2003.6.3 by M.Doi
�C��:2003.6.26 by Shunsuke Okamoto

cc fft.c -o fft -lm
***********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define PI 3.14159265358979323846
/*
  �֐�{\tt fft()}�̉������Ƃ��ĎO�p�֐��\�����.
*/

static void make_sintbl(int n, float sintbl[])
{
	int i, n2, n4, n8;
	double c, s, dc, ds, t;
	n2 = n / 2;  n4 = n / 4;  n8 = n / 8;
	t = sin(PI / n);
	dc = 2 * t * t;  ds = sqrt(dc * (2 - dc));
	t = 2 * dc;  c = sintbl[n4] = 1;  s = sintbl[0] = 0;
	for (i = 1; i < n8; i++) {
		c -= dc;  dc += t * c;
		s += ds;  ds -= t * s;
		sintbl[i] = (float)s;
		sintbl[n4 - i] = (float)c;
	}
	if (n8 != 0) sintbl[n8] = (float)sqrt(0.5);
	for (i = 0; i < n4; i++)
		sintbl[n2 - i] = sintbl[i];
	for (i = 0; i < n2 + n4; i++)
		sintbl[i + n2] = - sintbl[i];
}
/*
  �֐�{\tt fft()}�̉������Ƃ��ăr�b�g���]�\�����.
*/
static void make_bitrev(int n, int bitrev[])
{
	int i, j, k, n2;
	n2 = n / 2;  i = j = 0;
	for ( ; ; ) {
		bitrev[i] = j;
		if (++i >= n) break;
		k = n2;
		while (k <= j) {  j -= k;  k /= 2;  }
		j += k;
	}
}
/*
  ����Fourier�ϊ� (Cooley--Tukey�̃A���S���Y��).
  �W�{�_�̐� {\tt n} ��2�̐�����Ɍ���.
  {\tt x[$k$]} ������, {\tt y[$k$]} ������ ($k = 0$, $1$, $2$,
  \ldots, $|{\tt n}| - 1$).
  ���ʂ� {\tt x[]}, {\tt y[]} �ɏ㏑�������.
  ${\tt n} = 0$ �Ȃ�\�̃��������������.
  ${\tt n} < 0$ �Ȃ�t�ϊ����s��.
  �O��ƈقȂ� $|{\tt n}|$ �̒l�ŌĂяo����,
  �O�p�֐��ƃr�b�g���]�̕\����邽�߂ɑ����]���Ɏ��Ԃ�������.
  ���̕\�̂��߂̋L���̈�l���Ɏ��s�����1��Ԃ� (����I����
  �̖߂�l��0).
  �����̕\�̋L���̈���������ɂ� ${\tt n} = 0$ �Ƃ���
  �Ăяo�� (���̂Ƃ��� {\tt x[]}, {\tt y[]} �̒l�͕ς��Ȃ�).
*/
int fft(int n, float x[], float y[])
{
	static int    last_n = 0;    /* �O��ďo������ {\tt n} */
	static int   *bitrev = (int*) NULL; /* �r�b�g���]�\ */
	static float *sintbl = (float*) NULL; /* �O�p�֐��\ */
	int i, j, k, ik, h, d, k2, n4, inverse;
	float t, s, c, dx, dy;
	/* ���� */
	if (n < 0) {
		n = -n;  inverse = 1;  /* �t�ϊ� */
	} else inverse = 0;
	n4 = n / 4;
	if (n != last_n || n == 0) {
		last_n = n;
		if (sintbl != NULL) free((float*)sintbl);
		if (bitrev != NULL) free((int*)bitrev);
		if (n == 0) return 0;  /* �L���̈��������� */
		sintbl = (float*)malloc((n + n4) * sizeof(float));
		bitrev = (int*)malloc(n * sizeof(int));
		if (sintbl == NULL || bitrev == NULL) {
			fprintf(stderr, "�L���̈�s��\n");  return 1;
		}
		make_sintbl(n, sintbl);
		make_bitrev(n, bitrev);
	}
	for (i = 0; i < n; i++) {    /* �r�b�g���] */
		j = bitrev[i];
		if (i < j) {
			t = x[i];  x[i] = x[j];  x[j] = t;
			t = y[i];  y[i] = y[j];  y[j] = t;
		}
	}
	for (k = 1; k < n; k = k2) {    /* �ϊ� */
		h = 0;  k2 = k + k;  d = n / k2;
		for (j = 0; j < k; j++) {
			c = sintbl[h + n4];
			if (inverse) s = - sintbl[h];
			else         s =   sintbl[h];
			for (i = j; i < n; i += k2) {
				ik = i + k;
				dx = s * y[ik] + c * x[ik];
				dy = c * y[ik] - s * x[ik];
				x[ik] = x[i] - dx;  x[i] += dx;
				y[ik] = y[i] - dy;  y[i] += dy;
			}
			h += d;
		}
	}
	if (! inverse)    /* �t�ϊ��łȂ��Ȃ�n�Ŋ��� */
		for (i = 0; i < n; i++) {  x[i] /= n;  y[i] /= n;  }
	return 0;  /* ����I�� */
}

#define    N 1024
#define delta 20.0000E-06

int main(void)
{
	FILE *fpr,*fpw;
	int i;
	static float  x1[N], y1[N], x2[N], y2[N], res[N];
	double pk;

	printf("*** FFT ***\n");	

	//�M���ǂݍ���
	fpr = fopen("voiceh.csv","r"); 

	for(i=0;i<N;i++){
		fscanf(fpr,"%f",&x1[i]);

	    x2[i]=x1[i];
            y1[i]=y2[i]=0;
	}

	//FFT
	if (fft(N, x2, y2)) return EXIT_FAILURE;

	//�M�������o��
	fpw = fopen("spectrum.dat","w");

	printf("\n No.      INPUT DATA         FFT\n");
	printf("         Real Imagenary    Real Imagenary Power\n");
	for (i = 0; i < N/2-1; i++){
	  pk = delta*delta*(x2[i]*x2[i]+y2[i]*y2[i]);
	  pk = 10*log10(pk);
	  printf("%f %f %f\n",x1[i],y1[i],pk);
		fprintf(fpw,"%f,%f\n",x2[i],y2[i]); // real-part imaginary-part
	}

	fclose(fpr);
	fclose(fpw);
	return 0;
}
