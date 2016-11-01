#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define DATA_MAX 1024
#define pi 3.14159265359
#define delta 20.0000E-06

int main(void) {
  int i, data_count,n,k,N=DATA_MAX;
  FILE *fp,*out;
  char file_name[] = "voiceh.csv";
  double csv[DATA_MAX],ReF,ImF;
  double pk=0,fk=0,yokojiku;

  out = fopen("voice.dat", "w");

  /* ファイルオープン */
  if ((fp = fopen(file_name, "r")) == NULL) {
    fprintf(stderr, "%s\n", "error: can't read file.");
    return EXIT_FAILURE;
  }

    /* サンプルファイルを読み込む */
    i = 0;
    while (fscanf(fp, "%lf,,,", &csv[i]) != EOF){
        i++;
    }
    data_count = i;

    yokojiku = 1/(N*delta);

  for(k=0;k<=N/2-1;k++){
    ReF=ImF=0.0;
    for(n=0;n<N;n++) {
      ReF+=csv[n]*cos(2*pi*k*n/N);
      ImF+=-csv[n]*sin(2*pi*k*n/N);
    }
    pk = delta*delta*(ReF*ReF+ImF*ImF);
    pk = 10*log10(pk);
    fk += yokojiku;
    fprintf(out,"%f %f\n",fk,pk);
  }

  fclose(fp);
  fclose(out);

 
    return 0;
}
