/* RCGA code used to optimize fsi simulation
Jiujiu Lou, 07/12/2018

Objective function:
fitness = V/E (subject to change)

Methods proposed: 
tournament selection method 
laplace crossover/ BLX-alpha crossover
power mutation
penalty for out of constraint variables 

Possible modification:
instead of having to store two generations, dissecting the current
generation into two parts and keep portion of the current generation
while updating the rest. This will save computation of copying generations
at every iteration. */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include "RCGAheader.h"


int main(int argc, char* argv[]){
	// needs to import currentGen, Maxiter, PPL_size
	
	if (argc!=6){
		printf("not enough arguments\n");
		return 1;
	}
	int i;
	GEN_NUM = atoi(argv[1]);
	MAXITER = atoi(argv[2]);
	PPL_SIZE = atoi(argv[3]);
	CROSS = atoi(argv[4]); // 0 for BLX, 1 for laplace
	SELEC = atoi(argv[5]); // 0 for tournament, 1 for roulette
	
	///////// initialize variables ////////////////
	srand((unsigned)time(NULL));

	TOUR_SIZE = PPL_SIZE*TOUR_PERCENT;
	fworst = INFINITY;
	deltaA = deltaA_i+(deltaA_f-deltaA_i)*GEN_NUM/MAXITER;
	deltaF = deltaF_i+(deltaF_f-deltaF_i)*GEN_NUM/MAXITER;
	total_idx=(int*)malloc(sizeof(int)*(PPL_SIZE));
	tour_idx=(int*)malloc(sizeof(int)*(TOUR_SIZE));
	for (i=0;i<PPL_SIZE;i++)
		total_idx[i] = i;


	///////// obtain fitness values of current generation ////////
	ogen=(individual*)malloc(sizeof(individual)*(PPL_SIZE));
	ngen=(individual*)malloc(sizeof(individual)*(PPL_SIZE));
	for (i=0;i<PPL_SIZE;i++){
		readvalues(i); 
		feval(i);
		if (fworst>ogen[i].fit && ogen[i].fit > -INFINITY){
			fworst=ogen[i].fit;
		}
	}
	ffworst = fworst; // update ffworst
	// take care of out of bounds runs 
	for (i=0;i<PPL_SIZE;i++){
		if (ogen[i].fit==-INFINITY){
			ogen[i].fit=fworst-penval(i);
			if (ffworst>ogen[i].fit){
				ffworst=ogen[i].fit;
			}
		}
	}
	// take care of invalid runs
	for (i=0;i<PPL_SIZE;i++){
		if (ogen[i].fit!=ogen[i].fit){
			ogen[i].fit = ffworst;
		}
	}

	homogeneitycal();
	postvalues();
	logvalues();
	///////// perform GA and output next generation inputs /////////
	if (GEN_NUM<MAXITER){
		for (i=0;i<PPL_SIZE;i++){
			// selection method 
			switch(SELEC){
				case 0:
				updatetour();
				parent1 = maxfit(tour_idx,TOUR_SIZE);
				updatetour();
				parent2 = maxfit(tour_idx,TOUR_SIZE);
				break;
				case 1:
				parent1 = roulette();
				parent2 = roulette();
				break;
				default:
				parent1 = roulette();
				parent2 = roulette();				
			}

			// crossover
			switch(CROSS){
				case 0: BLX_alpha(i);
				break;
				case 1: laplacex(i);
				break;
				default: laplacex(i);
			}
			// mutate
			uniformutate(i,deltaA,deltaF);
			writevalues(i);
		}
	}
	return 0;
}

void printarrayi(int *arr,int size){
	/* print a 1D array, sperated by periods */
	int i;
	for (i=0;i<size;i++){
		printf("%i.", arr[i]);
	}
	printf("\n");
	return;
}

void readvalues(int idx){
	/* read A F E V from file */
	char input[255];
	char output[255];
	sprintf(input,"inputs/gen%d/input%d.%d",GEN_NUM,GEN_NUM,idx+1);
	sprintf(output,"outputs/gen%d/output%d.%d",GEN_NUM,GEN_NUM,idx+1);
	FILE *fp,*fp1;
	fp = fopen(input,"r");
	fp1 = fopen(output,"r");
	fscanf(fp,"%lf,%lf",&ogen[idx].A,&ogen[idx].F);
	fscanf(fp1,"%lf,%lf",&ogen[idx].P,&ogen[idx].V);
	fclose(fp);
	fclose(fp1);
	return;
}

void postvalues(void){
	/* output fitness and homogeneity values in txt file */
	char result[255]; 
	int i;
	int max = maxfit(total_idx, PPL_SIZE);
	sprintf(result,"outputs/gen%d/result%d",GEN_NUM,GEN_NUM);
	FILE *fp;
	fp = fopen(result,"w");
	fprintf(fp,"homogeneity:%lf,Best:%d,%lf.\nFitness Values:\n",homogeneity,max+1,ogen[max].fit);
	fclose(fp);
	fp = fopen(result,"a");
	for (i=0;i<PPL_SIZE;i++){
	fprintf(fp,"%f\n", ogen[i].fit);		
	}
	fclose(fp);
	return;
}

void writevalues(int idx){
	/* write A and F values for next iteration */
	char input[255];
	sprintf(input,"inputs/gen%d/input%d.%d",GEN_NUM+1,GEN_NUM+1,idx+1);
	FILE *fp;
	fp = fopen(input,"w");
	fprintf(fp, "%lf,%lf\n",ngen[idx].A,ngen[idx].F);
	fclose(fp);
	return;
}
				
int fit(int idx){
	/* check if the inputs fit the boundary, if not which one */
	if (ogen[idx].A<Amax && ogen[idx].A>Amin 
		&& ogen[idx].F<Fmax && ogen[idx].F>Fmin){
		return 1;
	}
	return 0;
}

void feval(int idx){
	/* evaluate the ogen fitness */
	// nan will be assigned to invalid runs and -inf to out of bounds
	if (ogen[idx].P!=ogen[idx].P||ogen[idx].V!=ogen[idx].V){
		ogen[idx].fit=0.0/0.0;
		return;
		
	}
	else {
		if (fit(idx)){
	////////////////////////// fitness /////////////////////
			ogen[idx].fit=pow(ogen[idx].V,2.75)/ogen[idx].P;
		}
		else{
			ogen[idx].fit=-INFINITY;
		}
	}
	return;
}				

double penval(int idx){
	double penalty = 0;
	if (ogen[idx].A<Amin)
		penalty += (Amin-ogen[idx].A)/(Amax-Amin);
	if (ogen[idx].A>Amax)
		penalty += (ogen[idx].A-Amax)/(Amax-Amin);
	if (ogen[idx].F<Fmin)
		penalty += (Fmin-ogen[idx].F)/(Fmax-Fmin);
	if (ogen[idx].F>Fmax)
		penalty += (ogen[idx].F-Fmax)/(Fmax-Fmin);
	return penalty;
}

int maxfit(int *idxarr, int size){
	int max = 0;
	int i;
	for (i=0;i<size;i++){
		if (ogen[max].fit<ogen[idxarr[i]].fit){
			max=idxarr[i];
		}
	}
	return max;
}

void updatetour(void){
	/* update tour indices */
	int fill = 0;
	int idx = 0;
	while (fill<TOUR_SIZE){
		//printf("%i\n",fill);
		if (randomgen(0,1)<TOUR_PERCENT){
			tour_idx[fill] = idx;
			fill++;
		}
		idx ++;
		if (idx>=PPL_SIZE){
			idx = 0;
		}
	}
	return;
}

double sum(int *idxarr, int size){
	double summ = 0;
	int i;
	for (i=0;i<size;i++){
		summ += ogen[idxarr[i]].fit;
	}
	return summ;
}


int roulette(void){
	/* find an index that corresponds to selected individual */
	
	double fitsum,randomp,stackp;
	int i, outindex;
	fitsum = sum(total_idx,PPL_SIZE);
	randomp = randomgen(0,1);
	stackp = 0.0;

	for (i=0; i<PPL_SIZE; i++){
		stackp += ogen[i].fit/fitsum;
		//printf("stackp is %f\n", stackp);
		if (stackp >= randomp){
			outindex = i;
			return outindex;
		}
	}
}

void BLX_alpha(int idx){
	/* use BLX crossover to update individuals at idx and idx+1 */
	double cmin, cmax, I;
	if (randomgen(0,1)<CO_PROB){
		// A
		if (ogen[parent1].A>ogen[parent2].A){
			cmax = ogen[parent1].A;
			cmin = ogen[parent2].A;
		}
		else{
			cmax = ogen[parent2].A;
			cmin = ogen[parent1].A;
		}
		I = cmax - cmin;
		ngen[idx].A = randomgen(minmax(cmin-I*alpha,Amin,1),minmax(cmax+I*alpha,Amax,0));
		// F
		if (ogen[parent1].F>ogen[parent2].F){
			cmax = ogen[parent1].F;
			cmin = ogen[parent2].F;
		}
		else{
			cmax = ogen[parent2].F;
			cmin = ogen[parent1].F;
		}
		I = cmax - cmin;
		ngen[idx].F = randomgen(minmax(cmin-I*alpha,Fmin,1),minmax(cmax+I*alpha,Fmax,0));
	}
	else {
		ngen[idx].A = ogen[parent1].A;
		ngen[idx].F = ogen[parent1].F;
	}
	return;
}

void laplacex(int idx){
	/*a & b are laplace distribution parameters, 
	idx is index for first offspring */
	double beta;
	if (randomgen(0,1)>CO_PROB){
		ngen[idx].A = ogen[parent1].A;
		ngen[idx].F = ogen[parent1].F;
	}
	else {
		beta = laplacea + pow(-1,rand()%2)*laplaceb*log(randomgen(0,1));
		ngen[idx].A = ogen[parent1].A+beta*abs(ogen[parent1].A-ogen[parent2].A);
		ngen[idx].F = ogen[parent1].F+beta*abs(ogen[parent1].F-ogen[parent2].F);
	}
	return;
}


void uniformutate(int idx, double deltaA, double deltaF){
	if (randomgen(0,1)<MU_PROB){
		if (rand()%2==0){
			ngen[idx].A += randomgen(0,1)*deltaA;
		}
		else {
			ngen[idx].A -= randomgen(0,1)*deltaA;
		}
		if (rand()%2==0){
			ngen[idx].F += randomgen(0,1)*deltaF;
		}
		else {
			ngen[idx].F -= randomgen(0,1)*deltaF;
		}
	}
	return;	
}

void homogeneitycal(void){
	/* calculate the homogeneity of current population */
	double repeat = 0;
	double max = ogen[maxfit(total_idx,PPL_SIZE)].fit;
	int i;
	for (i=0;i<PPL_SIZE;i++){
		if ((max-ogen[i].fit)<1e-6){
			repeat ++;
		}
	}
	homogeneity = repeat/PPL_SIZE;
	return;
}

void logvalues(void){
	char logfile[255];
	int i;
	sprintf(logfile,"logged/logged.%d.%d.%d",GEN_NUM,CROSS,SELEC);
	FILE *fplog;
	fplog = fopen(logfile,"w");
	for (i=0;i<PPL_SIZE;i++){
		fprintf(fplog, "%lf,%lf,%lf\n",ogen[i].A,ogen[i].F,ogen[i].fit);
	}
	fclose(fplog);
}
