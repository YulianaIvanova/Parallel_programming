#include "mpi.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include "time.h"
using namespace std;

// MPI routines
int ProcNum, ProcRank;

void Flip (double *B, int Size) 
{ 
	double temp=0.0; 
	for (int i=0;i<Size;i++)
	{
		for (int j=i+1;j<Size;j++)
		{
			temp = B[i*Size+j];
			B[i*Size+j] = B[j*Size+i];
			B[j*Size+i] = temp;
		}
	}
}

void multi_matrixMPI(double* a, double* b, double* c, int size_matrix)
{
	int dim = size_matrix;
        int i, j, k, p, ind;
	double temp;
	MPI_Status Status;
	int ProcPartSize = dim/ProcNum; 
	int ProcPartElem = ProcPartSize*dim;
	double* bufA = new double[ProcPartElem];
	double* bufB = new double[ProcPartElem];
	double* bufC = new double[ProcPartElem];
	int ProcPart = dim/ProcNum, part = ProcPart*dim;
	if (ProcRank == 0) {
		Flip(b, size_matrix);
	}
	
	MPI_Scatter(a, part, MPI_DOUBLE, bufA, part, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Scatter(b, part, MPI_DOUBLE, bufB, part, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	temp = 0.0;
	for (i=0; i < ProcPartSize; i++) {
		for (j=0; j < ProcPartSize; j++) {
			for (k=0; k < dim; k++) 
				temp += bufA[i*dim+k]*bufB[j*dim+k];
			bufC[i*dim+j+ProcPartSize*ProcRank] = temp;
			temp = 0.0;
		}
	}

        int NextProc; int PrevProc;
	for (p=1; p < ProcNum; p++) {
		NextProc = ProcRank+1;
		if (ProcRank == ProcNum-1) 
			NextProc = 0;
		PrevProc = ProcRank-1;
		if (ProcRank == 0) 
			PrevProc = ProcNum-1;
		MPI_Sendrecv_replace(bufB, part, MPI_DOUBLE, NextProc, 0, PrevProc, 0, MPI_COMM_WORLD, &Status);
		temp = 0.0;
		for (i=0; i < ProcPartSize; i++) {
			for (j=0; j < ProcPartSize; j++) {
				for (k=0; k < dim; k++) {
					temp += bufA[i*dim+k]*bufB[j*dim+k];
				}
				if (ProcRank-p >= 0 ) 
					ind = ProcRank-p;
				else ind = (ProcNum-p+ProcRank);
				bufC[i*dim+j+ind*ProcPartSize] = temp;
				temp = 0.0;
			}
		}
	}

	MPI_Gather(bufC, ProcPartElem, MPI_DOUBLE, c, ProcPartElem, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	delete []bufA;
	delete []bufB;
	delete []bufC;
	
	MPI_Finalize();


	return;
}


int main(int argc, char *argv[])
{
	clock_t start;
	cout << "Begin calculating ..." << endl;

	const char* file_a = "./matrix_A/matrix_A_100.txt";
	const char* file_b = "./matrix_B/matrix_B_100.txt";
	const char* file_c = "./matrix_C/matrix_C_100.txt";
	ifstream in_a(file_a);
	int size_matrix = 0;

	in_a >> size_matrix;

	double *a =new double[size_matrix*size_matrix];
	
	for (int i = 0; i < size_matrix; i++)
		for (int j = 0; j < size_matrix; j++)
			in_a >> a[i*size_matrix+j];

	in_a.close();


	ifstream in_b(file_b);
	in_b >> size_matrix;

	double* b = new double[size_matrix*size_matrix];
	

	for (int i = 0; i < size_matrix; i++)
		for (int j = 0; j < size_matrix; j++)
			in_b >> b[i*size_matrix+j];

	in_b.close();

	cout << "Size: " << size_matrix << endl;

	double* c = new double[size_matrix*size_matrix];

	cout << "Begin calculating ..." << endl;
	
	start = clock();

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
        MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
	
	multi_matrixMPI(a,b,c,size_matrix);
	

	cout << endl << "Calculation time: " << double(clock() - start)/CLOCKS_PER_SEC << " seconds" << endl;
		
	ofstream of(file_c);
	if (of.is_open())
	{
		of << size_matrix << endl;
		for (int i = 0; i < size_matrix; i++)
		{
			for (int j = 0; j < size_matrix; j++)
			{
				of << c[i*size_matrix+j] << "\t";
			}

			of << "\n";
		}

		of.close();
	}
	else
	{
		cout << "File not found";
	}

	delete[] a;
	delete[] b;
	delete[] c;
	return 0;
}