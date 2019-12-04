#include <stdio.h>
#include <stdlib.h>
#include <string.h>

double* theta; 
double* torque; 
double* disp; 
double* P;
double* V;

double arrayaverage(double* arr,int size){
	double sum=0;
	int i;
	for (i=0;i<size;i++){
		sum += arr[i];
	}
	return sum/size;
}

int main(int argc, char const *argv[])
{
	int i;
	char stepfile[255];
	char name[255];
	char name1[255];
	char output[255];
	/* calculate velocity and energy */
	int gen, individual, flag, end_iter;
	double Frequency,delta_t;
	gen = atoi(argv[1]);
	individual = atoi(argv[2]);

	printf("\nNow evaluating generation %d,individual %d\n",gen,individual);
	// get iteration counts
	sprintf(stepfile,"Program_FIN%d/result/EVcalsupport",individual);
	FILE *fp;
	fp = fopen(stepfile,"r");
	fscanf(fp,"%d,%d,%lf,%lf",&flag,&end_iter,&Frequency,&delta_t);
	fclose(fp);

	// get theta, torque and disp
	theta = (double*)malloc(sizeof(double)*(end_iter));
	torque = (double*)malloc(sizeof(double)*(end_iter));
	disp = (double*)malloc(sizeof(double)*(end_iter));
	P = (double*)malloc(sizeof(double)*(end_iter-1));
	V = (double*)malloc(sizeof(double)*(end_iter-1));

	for (i=0;i<end_iter;i++){
		sprintf(name,"Program_FIN%d/result/disp/disp%d",individual,i+1);
 		sprintf(name1,"Program_FIN%d/result/r_force/r_force%d",individual,i+1);
	  	FILE *fp1,*fp2;
	  	fp1 = fopen(name,"r");
	  	fp2 = fopen(name1,"r");
	  	fscanf(fp1,"%lf %*f %lf",&disp[i],&theta[i]);
	  	fscanf(fp2,"%*f %lf",&torque[i]);
	  	//printf("%f,%f,%f\n",theta[i],torque[i],disp[i]);
	  	fclose(fp1);
	  	fclose(fp2);
	}

	if (flag){
		// in case boundary is not reached (invalid run)
		sprintf(output,"outputs/gen%d/output%d.%d",gen,gen,individual);
	  	FILE *fp3;
	  	fp3 = fopen(output,"w");
	  	fprintf(fp3,"%f,%f",0.0/0.0,0.0/0.0);
	  	printf("Boundary not reached, inf values assigned..\n");
	  	fclose(fp3);
	  	return 0;
	}


	for (i=0;i<end_iter-1;i++){
		V[i] = -(disp[i+1]-disp[i])/delta_t;
		P[i] = (torque[i+1]+torque[i])
			*(theta[i+1]-theta[i])/2/(delta_t);
	}

	double aveV, aveP;
	aveV = arrayaverage(V,end_iter-1);
	aveP = arrayaverage(P,end_iter-1);
	sprintf(output,"outputs/gen%d/output%d.%d",gen,gen,individual);
  	FILE *fp3;
  	fp3 = fopen(output,"w");
  	fprintf(fp3,"%f,%f",aveP,aveV);
  	fclose(fp3);
  	printf("Finished, average P: %lf, average V: %lf.\n\n",aveP,aveV);
	return 0;
}
