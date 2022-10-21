#include <fstream>
#include <iostream>

using namespace std;

void create_matrix(const char* file_name, int size_matrix)
{
	ofstream of(file_name);

	if (of.is_open())
	{
		if (size_matrix > 1)
		{
			of << size_matrix << endl;
			for (int i = 0; i < size_matrix; i++)
			{
				for (int j = 0; j < size_matrix; j++)
				{
					of << rand() % 101 << "\t";
				}

				of << "\n";
			}
		}
		else
		{
			cout << "The size of the matrix must be greater than 1" << endl;
		}

		of.close();
	}
	else
	{
		cout << "File not found" << endl;
	}

	return;
}

int main()
{
	create_matrix("matrix_A_1100.txt", 1100);
	create_matrix("matrix_B_1100.txt", 1100);
	return 0;
}