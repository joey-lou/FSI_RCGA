// gather data to be plotted on matlab, further analyze the E/V calculations

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(int argc, char const *argv[])
{
	double A, F, reaction, theta, torque, disp;
	int gen, ind, enditer, i, count;
	// Specify the generation and individual we want to plot from
	// KEEP IN MIND: generation must be the most recent one
	// we can not go back to previous generations because data in 
	// Program_FIN will be updated already.

	gen = atoi(argv[1]);
	ind = atoi(argv[2]);
	char input[255];
	char output[255];
	char dispf[255];
	char rforce[255];
	char helper[255];
	char temp[255];
	FILE *fp1,*fp2,*fp3,*fp4,*fp5;
	// get amplitude and frequency first
	sprintf(input,"inputs/gen%d/input%d.%d",gen,gen,ind);
	fp1=fopen(input,"r");
	fscanf(fp1,"%lf,%lf",&A,&F);
	fclose(fp1);

	// get max iteration
	sprintf(helper,"Program_FIN%d/result/EVcalsupport",ind);
	fp2=fopen(helper,"r");
	fscanf(fp2,"%*d,%d",&enditer);
	fclose(fp2);	


	sprintf(output,"matlabdata/data%.3f%.3f",A,F);
	fp3=fopen(output,"w");
	
	for (i=1;i<=enditer;i++){
		count = 1;
		sprintf(dispf,"Program_FIN%d/result/disp/disp%d",ind,i);
		sprintf(rforce,"Program_FIN%d/result/r_force/r_force%d",ind,i);
		fp4 = fopen(dispf,"r");
		fp5 = fopen(rforce,"r");
		fscanf(fp4,"%lf %*f %lf",&disp,&theta);
		while (count<44){
			fgets(temp, sizeof temp, fp5);
			if (count==2)
				torque=atof(temp);
			if (count==43)
				reaction=atof(temp);
			count++;
		}
		fprintf(fp3,"%lf,%lf,%lf,%lf\n",theta,torque,disp,reaction);	
		fclose(fp4);
		fclose(fp5);
	}
	fclose(fp3);
	return 0;
}