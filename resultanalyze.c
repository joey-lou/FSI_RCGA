/* Analyze the result outputs from RCGA runs on FSI */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>


int main(int argc, char const *argv[])
{	/*
	if (argc!=3){
		printf("not enough arguments\n");
		return 1;
	}*/
	int i,j,ID,count,MAXITER,PPL_SIZE,CROSS,SELEC;
	double homogeneity,fitness,A,F;

	MAXITER = atoi(argv[1]);
	PPL_SIZE = atoi(argv[2]);
	CROSS = atoi(argv[3]);
	SELEC = atoi(argv[4]);

	//MAXITER = 1;
	//PPL_SIZE = 5;
	char result[255];
	char temp[255];
	char output[255];
	sprintf(output, "matlabdata/plot%d%d",CROSS,SELEC);
	FILE *fpp;
	fpp = fopen(output,"w");
	for (i=1;i<=MAXITER;i++){
		sprintf(result,"outputs/gen%d/result%d",i,i);
		FILE *fp;
		char line[255];
		fp = fopen(result,"r");
		fscanf(fp,"homogeneity:%lf,Best:%d,%lf.\nFitness Values:",&homogeneity,&ID,&fitness);
		printf("Gen[%d] homogeneity:%lf\t\tBest[%d]:F=%lf",i,homogeneity,ID,fitness);
		sprintf(temp,"inputs/gen%d/input%d.%d",i,i,ID);
		FILE *fp1;
		fp1 = fopen(temp,"r");
		fscanf(fp1,"%lf,%lf",&A,&F);
		fclose(fp1);
		printf(",A=%lf,F=%lf\n",A,F);

		count = 1;
		printf("[\t");
		while (fgets(line, sizeof line, fp) != NULL){
			if (count>1){
				printf("%lf\t",atof(line));
			}
			count++;
		}
		printf("]\n");
		fclose(fp);
		fprintf(fpp, "%d,%lf\n",i,fitness);
	}
	fclose(fpp);

	return 0;
}