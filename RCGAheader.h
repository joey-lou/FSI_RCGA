/* constants */
const float CO_PROB = 0.8;						// crossover probability
const float MU_PROB = 0.2;						// mutation probability
const float TOUR_PERCENT = 0.2;				// percentage of population enters tournaments
const double Amin=0.1;
const double Amax=1.0;
const double Fmin=2.0;
const double Fmax=10.0;
const double alpha=1.5;							// BLX crossover parameter
const float deltaA_i = 0.1;					// mutate parameter
const float deltaA_f = 0.00;					// mutate parameter
const float deltaF_i = 0.8;						// mutate parameter
const float deltaF_f = 0.00;
const double laplacea = 0.00;					// laplace crossover parameter
const double laplaceb = 1.0;					// laplace crossover parameter


/* global variables */
typedef struct individuals{
	double A;
	double F;
	double V;
	double P;
	double fit;
} individual;
int MAXITER;     					// get as argument input 
int PPL_SIZE;						// get as argument input 
int GEN_NUM;						// current generation number
int CROSS;
int SELEC;

int TOUR_SIZE;		 
int parent1, parent2;				// parents selected from old generation
int* tour_idx;						// indices of those enter tournaments
double fworst;			// the worst feval result from previous generation
double ffworst;			// the worst feval after penalty added, reserved for invalid runs
int* total_idx;
double homogeneity;
double deltaA;
double deltaF;
individual* ogen;
individual* ngen;


/* helper function list */
void readvalues(int idx);
void writevalues(int idx);
void postvalues(void);
double sum(int *idxarr, int size);

int maxfit(int *idxarr, int size);				// returns index of max in array
int fit(int idx);								// check if indexed individual fits constraints
void feval(int idx);							// evaluate function value for indexed individual
double penval(int idx);							// evaluate penalty added to indexed individual
void updatetour(void);							// update indices in tournament
int roulette(void);
void homogeneitycal(void);
double randomgen(double minimum, double maximum){
	double ans = (double)rand()/RAND_MAX*(maximum-minimum);
	return ans+minimum;
}
double minmax(double num1, double num2, int flag){
	// flag 0 for finding min, 1 for max
	if (flag){
		if (num1>num2)
			return num1;
		return num2;
	}
	else{
		if (num1<num2)
			return num1;										
		return num2;
	} 
}


/* crossover methods */
void BLX_alpha(int idx);
void laplacex(int idx);
/* mutation methods */
void uniformutate(int idx, double deltaA, double deltaF);

/* visualization tools */
void printarrayi(int *arr,int size);
void logvalues(void);
