#include <iostream>
#include <cstdlib>
#include <mpi.h>
using namespace std;

int** allocateSquareMatrix( int size );
int** matrixMultiply( int** A, int** B, int size );

int main( int argc, char** argv )
{
	MPI_Init( &argc, &argv );

	if( argc < 2 || argc > 2 )
	{
		cout << "Usage: main [size]" << endl;
		return 1;
	}

	int size = atoi(argv[1]);

	int** A = allocateSquareMatrix( size );
	int** B = allocateSquareMatrix( size );

	double start_time = MPI_Wtime( );
	int ** result = matrixMultiply( A, B, size );
	double end_time = MPI_Wtime( );

	printf( "%d,%.6f\n", size, end_time - start_time );
	
	MPI_Finalize( );
	return 0;
}

int** allocateSquareMatrix( int size )
{
	int** result = new int*[size];
	for( int i = 0; i < size; i++ )
	{
		result[i] = new int[size];
	}
	return result;
}

int** matrixMultiply( int** A, int** B, int size )
{
	int i, j, k, sum;
	int** result = allocateSquareMatrix( size );

	for( i = 0; i < size; i++ )
	{
		for( j = 0; j < size; j++ )
		{
			sum = 0;
			for( k = 0; k < size; k++ )
			{
				sum = sum + A[i][k] * B[k][j];
			}
			result[i][j] = sum;
		}
	}

	return result;
}