#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

int main(int argc, char const *argv[])
{
	// inputs: gen#, ppl_size
	int gen,ppl_size,i;
	double A,F;
	gen = atoi(argv[1]);
	ppl_size =  atoi(argv[2]);
	char input[255];
	FILE *fp;
	printf("Gen%d:\n", gen);
	for (i=1;i<=ppl_size;i++){
		sprintf(input,"inputs/gen%d/input%d.%d",gen,gen,i);
		fp = fopen(input,"r");
		fscanf(fp,"%lf,%lf",&A,&F);
		printf("[%d]: A=%lf,F=%lf.\n",i,A,F);
		fclose(fp);
	}
	return 0;
}