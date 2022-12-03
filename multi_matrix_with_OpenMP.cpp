#include <fstream>
#include <iostream>
#include <cstdlib>
#include "time.h"
#include <omp.h>
using namespace std;

void multi_matrix(const char* file_a, const char* file_b)
{
	
	ifstream in_a(file_a);
	int size_matrix = 0;

	in_a >> size_matrix;
	double** a;
	a = new double* [size_matrix];
	for (int i = 0; i < size_matrix; i++) a[i] = new double[size_matrix];

	for (int i = 0; i < size_matrix; i++)
		for (int j = 0; j < size_matrix; j++)
			in_a >> a[i][j];

	in_a.close();


	ifstream in_b(file_b);
	in_b >> size_matrix;
	double** b;
	b = new double* [size_matrix];
	for (int i = 0; i < size_matrix; i++) b[i] = new double[size_matrix];

	for (int i = 0; i < size_matrix; i++)
		for (int j = 0; j < size_matrix; j++)
			in_b >> b[i][j];

	in_b.close();

	cout << "Size: " << size_matrix << endl;

	double** c;
	c = new double* [size_matrix];
	for (int i = 0; i < size_matrix; i++) c[i] = new double[size_matrix];

	clock_t start;
	cout << "Begin calculating ..." << endl;
	start = clock();

	double wtime = omp_get_wtime();

	int threads;
	omp_set_num_threads(16);
	#pragma omp parallel shared(threads)
	{
		threads = omp_get_num_threads();
	
	#pragma omp for
		for (int i = 0; i < size_matrix; i++)
		{
			for (int j = 0; j < size_matrix; j++)
			{
				c[i][j] = 0;
				for (int k = 0; k < size_matrix; k++)
					c[i][j] += a[i][k] * b[k][j];
			}
		}
	}
	
	wtime = omp_get_wtime() - wtime;
	cout << "Elapsed omp time: " << wtime << " seconds" << endl;
	cout << endl << "Num of threads: " << threads << endl << "Calculation time: " << double(clock() - start) / CLOCKS_PER_SEC << " seconds" << endl;

	ofstream of("./matrix_C/matrix_C_1200.txt");
	if (of.is_open())
	{
		of << size_matrix << endl;
		for (int i = 0; i < size_matrix; i++)
		{
			for (int j = 0; j < size_matrix; j++)
			{
				of << c[i][j] << "\t";
			}

			of << "\n";
		}

		of.close();
	}
	else
	{
		cout << "File not found";
	}

	for (int i = 0; i < size_matrix; i++) delete[] a[i];
	delete[] a;
	for (int i = 0; i < size_matrix; i++) delete[] b[i];
	delete[] b;
	for (int i = 0; i < size_matrix; i++) delete[] c[i];
	delete[] c;
	return;
}


int main()
{
	multi_matrix("./matrix_A/matrix_A_1200.txt", "./matrix_B/matrix_B_1200.txt");
	return 0;
}