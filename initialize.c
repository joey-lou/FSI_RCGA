#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "RCGAheader.h"


int main(int argc, char const *argv[])
{
	/* initializing script */
	// Ranges for Amplitude and Frequency
	int i;
	char input[255];
	srand((unsigned)time(NULL));
  	for (i=1;i<=atoi(argv[1]);i++){
	  	sprintf(input,"inputs/gen%d/input%d.%d",1,1,i);
	  	FILE *fp;
	  	fp = fopen(input,"w");
	  	fprintf(fp,"%lf,%lf",randomgen(Amin,Amax),randomgen(Fmin,Fmax));
	  	fclose(fp);
  	}
	return 0;
}

