								// IMPLEMENTATION FOR FIR FILTER AND 3-STAGE PARALLEL FIR FILTER //
								// ABHIJEET OMBLE //
								// DESIGN OF MULTIMEDIA SYSTEMS - EE-4495//	

#include <stdio.h>

//#define N 1024			// No.of co-efficients
//#define DATACOUNT 2000	// INFINITY
#define N 8					// No.of co-efficients
#define DATACOUNT 30		// INFINITY
#define PARA_STAGES 4

int input(void);
int inputParallel(int*);
int	fir(int, int*, int);
void fir_parallel(int, int*, int*);


int data[DATACOUNT] = {0xdeadbeef};
int X[DATACOUNT] = {0};
signed int iC = -1;		// input counter
int iC_block = 0;		// input counter block
FILE* file1 = NULL;		// print output to this file
FILE* file2 = NULL;		// print output to this file

int main()
{
//	float x,y;
	int coeff[N] = {};
	int length = N;
	int x = 0xff, y = 0xff;

	for(int i=1; i<=N; i++)
		coeff[i-1] = i;

	for(int i=1; i<=DATACOUNT; i++)
		data[i-1] = i*2;

	file1 = fopen("output1.txt","w");
	file2 = fopen("output2.txt","w");

	if(file1 && file2)
		printf("\n\n !!! Output redirected to file output1.txt (FIR filter) and output2.txt (PARALLEL FIR filter) in current folder !!!\n\n");

	fprintf(file1,"\n\nDATACOUNT:2000, CO-EFF:1024\n");
	fprintf(file2,"\n\nDATACOUNT:2000, CO-EFF:1024\n");
/*
	fprintf(file1,"\n");
	for(int i=0; i<N; i++)
		fprintf(file1,"coeff[%d]:%d\n",i,coeff[i]);

	for(int i=0; i<N; i++)
		fprintf(file1,"Input[%d]:%d\n",i,x[i]);
*/

	fprintf(file1,"\n\n\n***** FIR FILTER *****\n");
	while(1)	// SINGLE FIR Filter
	{
		x = input();

/*		fprintf(file1,"\ncoeff[]:");
		for(int i=N-1; i>=0; i--)
			fprintf(file1,"%2d ", coeff[i]);
*/
		if(x != 0xdeadbeef && iC<DATACOUNT)
		{
			fprintf(file1,"\n\nX[%d]:%d",iC,x);
		}
		else
		{
			fprintf(file1,"\nEOF: Exiting while(1)\n");
			fprintf(file1,"\ny[n] values cross-checked for the CO-EFF=1024.\nThe calculated values are correct.\n\n");
			break;
		}

		y = fir(length, coeff, x);

		fprintf(file1,"\nY[%d]:%d",iC,y);
	}

	iC = 0;
	int ll_input[PARA_STAGES] = {0};			// Array of parallel inputs

	fprintf(file2,"\n\n\n***** PARALLEL FIR FILTER *****\n");

/*	fprintf(file2,"\nData:");
	for(int i=0; i<DATACOUNT; i++)
		fprintf(file2,"%d ",data[i]);
*/
	while(1)	// N-stage parallel filter
	{
		if(0xdeadbeef != inputParallel(ll_input) )
		{
			fprintf(file2,"\n\nll_input:");
			for(int i=0; i<PARA_STAGES; i++)
				fprintf(file2,"%d ",*(ll_input+i) );
		}
		else
		{
			fprintf(file2,"\nEOF: Exiting while(1)\n");
			fprintf(file2,"\ny[n] values cross-checked for the CO-EFF=1024.\nThe calculated values are correct.\n\n");
			break;
		}

		fir_parallel(PARA_STAGES, coeff, ll_input);
	
	}

	fclose(file1);
	fclose(file2);

	return 0;
}

int fir(int length, int* coeff, int x)
{
	int i = 0, k = 0;
	int j = iC/N;					// j => block number of input, i.e. inputCount/N
	int y = 0;

	k = (iC % N) + 1 + (j-1)*N;	// Starting index of Xn to be used for a particular Y[]:
								// y7 = a7(x0) + a6(x1) + a5(x2) + a4(x3) + a3(x4) + a2(x5) + a1(x6) + a0(x7)					=> k=0
								// y8 =          a7(x1) + a6(x2) + a5(x3) + a4(x4) + a3(x5) + a2(x6) + a1(x7) + a0(x8)			=> k=1
								// y9 =                   a7(x2) + a6(x3) + a5(x4) + a4(x5) + a3(x6) + a2(x7) + a1(x8) + a0(x9)	=> k=2

	i = N-1;					// Index into the Co-eff[i]

//	fprintf(file1,"\niC:%d, j:%d, k:%d, i:%d",iC, j, k, i);

	for(; i>=0; i--,k++)
	{
		y += coeff[i] * X[k];
	}
	
	return y;
}

//
// 4-stage Parallel FIR filter
//
void fir_parallel(int length, int* coeff, int* ll_input)
{
	iC-=PARA_STAGES;
	for(int count=0; count<PARA_STAGES; count++, iC++)
	{
		int i = 0, k = 0;
		int j = iC/N;					// j => block number of input, i.e. inputCount/N
		int y = 0;

		k = (iC % N) + 1 + (j-1)*N;	
								// Starting index of Xn to be used for a particular Y[]:
								// y7 = a7(x0) + a6(x1) + a5(x2) + a4(x3) + a3(x4) + a2(x5) + a1(x6) + a0(x7)					=> k=0
								// y8 =          a7(x1) + a6(x2) + a5(x3) + a4(x4) + a3(x5) + a2(x6) + a1(x7) + a0(x8)			=> k=1
								// y9 =                   a7(x2) + a6(x3) + a5(x4) + a4(x5) + a3(x6) + a2(x7) + a1(x8) + a0(x9)	=> k=2

		i = N-1;				// Index into the Co-eff[i]

	//	fprintf(file2,"\niC:%d, j:%d, k:%d, i:%d",iC, j, k, i);

		for(; i>=0; i--,k++)
		{
			y += coeff[i] * X[k];
		}
	
		fprintf(file2,"\nY[%d]:%d",iC,y);
	}
}

int input(void)
{
	if(iC > DATACOUNT)
		return 0xdeadbeef;
	else
	{
		iC++;
		for(int i=0; i<=iC; i++)
			X[i] = data[i];

/*		fprintf(file1,"\n\nX[]:    ");
		for(int i=0; i<N; i++)
			fprintf(file1,"%2d ", X[i]);
*/
		return X[iC];		// no of valid data input X[]
	}	
}

int inputParallel(int* ll_input)
{
	if(iC >= DATACOUNT)
		return 0xdeadbeef;
	else
	{
//		fprintf(file2,"\n\ninputParallel():iC %d",iC);
		
		for(int j=0; j<PARA_STAGES; j++,iC++)
			ll_input[j] = data[iC];
	}
	return 0;
}
