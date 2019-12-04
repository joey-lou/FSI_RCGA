#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NN 6690
#define MM 12492
#define LINE_LENGTH_MAX 256 // １行の長さの上限
#define DAT_NUM_MAX 128     // データ数の上限

//int main(int argc, _TCHAR* argv[])
int main(int argc, char const *argv[])
{
  /* change made to accomodate different length */
  int gen,individual;
  gen = atoi(argv[1]);
  individual = atoi(argv[2]);
  char support[255];
  sprintf(support,"Program_FIN%d/result/EVcalsupport",individual);
  FILE *fp1;
  fp1 = fopen(support,"r");
  int enditer;
  fscanf(fp1,"%*d,%d",&enditer);
  fclose(fp1);
  // original code
  double xy[NN][2];
  int yo[MM][3];
  FILE *fp,*fpp;
  char line[LINE_LENGTH_MAX];
  int a,b,c,d,n,gyo,ret;
  double bx,cx,dx;
  double vector[NN][2];
  double scalor[NN];
  gyo=0;
  char fluidfile[255];
  sprintf(fluidfile,"Program_FIN%d/result/fluid_nop",individual);
  fpp=fopen(fluidfile,"r");
  while((ret=fscanf(fpp,"%d %d %d",&b,&c,&d))!=EOF){
    yo[gyo][0]=b;
    yo[gyo][1]=c;
    yo[gyo][2]=d;
    gyo++;
  }    
  fclose (fpp);
  gyo=0;
  sprintf(fluidfile,"Program_FIN%d/result/fluid_coord",individual);
  fpp=fopen(fluidfile,"r");
  while((ret=fscanf(fpp,"%lf %lf",&bx,&cx))!=EOF){
    xy[gyo][0]=bx;
    xy[gyo][1]=cx;
    gyo++;
  }    
  fclose (fpp);
	printf("Making vtk movie..\n");
  for(a=1;a<=enditer;a++){
    double time;
    time=a*0.01; // changed from 0.02
    char filename0[255];
    char filename1[255];
    char filename2[255];
    sprintf(filename0,"Program_FIN%d/result/f_coord/f_coord%d",individual,a);
    sprintf(filename1,"Program_FIN%d/result/pressure/pressure%d",individual,a);
    sprintf(filename2,"vtk/gen%d/individual%d/vtk%d.vtk",gen,individual,a);

    gyo=0;
    fpp=fopen(filename0,"r");
    while((ret=fscanf(fpp,"%lf %lf",&cx,&dx))!=EOF){
      xy[gyo][0]=cx;
      xy[gyo][1]=dx;
      gyo++;
    }    
    fclose (fpp);
    gyo=0;
    fpp=fopen(filename1,"r");
    while((ret=fscanf(fpp,"%lf",&dx))!=EOF){
      scalor[gyo]=dx;
      gyo++;
    }    
    fclose (fpp);

    FILE *ffp;
    ffp=fopen(filename2,"w");
    int c=0; 
    int d,e;
    fprintf(ffp,"# vtk DataFile Version 2.0\r\n");
    fprintf(ffp,"label\r\n");
    fprintf(ffp,"ASCII\r\n");
    fprintf(ffp,"DATASET UNSTRUCTURED_GRID\r\n");
    fprintf(ffp,"POINTS %d float\r\n",NN);

    for(c=0;c<NN;c++){
      fprintf(ffp,"%f %f 0.00\r\n",xy[c][0],xy[c][1]);
    }

    fprintf(ffp,"CELLS %d %d\r\n",MM,MM*4);
    for(c=0;c<MM;c++){
      fprintf(ffp,"3 %d %d %d\r\n",yo[c][0],yo[c][1],yo[c][2]);
    }
    fprintf(ffp,"CELL_TYPES %d\r\n",MM);
    for(c=0;c<MM;c++){
      fprintf(ffp,"5\r\n");
    }
    /*
    fprintf(ffp,"POINT_DATA %d\r\n",NN);
    fprintf(ffp,"VECTORS point_vectors float\r\n");
    for(c=0;c<NN;c++){
      fprintf(ffp,"%f %f 0.00\r\n",vector[c][0],vector[c][1]);
    }
    */    
    
    fprintf(ffp,"POINT_DATA %d\r\n",NN);
    fprintf(ffp,"SCALARS point_vectors float\r\n");
    fprintf(ffp,"LOOKUP_TABLE default\r\n");    
    for(c=0;c<NN;c++){
      fprintf(ffp,"%f\r\n",scalor[c]);
    }
    
    fclose(ffp);

  }
  return 0;
}
