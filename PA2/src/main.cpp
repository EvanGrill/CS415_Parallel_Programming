#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <mpi.h>
using namespace std;

#include "PIMFuncs.hpp"

struct Color
{
	uint8_t red;
	uint8_t blue;
	uint8_t green;
};

vector<Color> readColors ( string filename )
{
	vector<Color> output;
	Color temp;
	fstream input( filename );
	
	if( input )
	{
		while( !input.eof( ) )
		{
			input >> temp.red;
			input >> temp.blue;
			input >> temp.green;
			output.push_back( temp );
		}
	}

	return output;
}

double map(double x, double in_min, double in_max, double out_min, double out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

uint8_t calc_pixel( int x, int y, int width, int height, int iterations )
{
	double x_0 = map( x, 0, width, -2, 2 );
	double y_0 = map( y, 0, height, -2, 2 );
	double x_test = 0.0;
	double y_test = 0.0;
	int i = 0;
	double x_temp;
	while( x_test * x_test + y_test * y_test < 4 &&
		   i < iterations )
	{
		x_temp = x_test * x_test - y_test * y_test + x_0;
		y_test = 2 * x_test * y_test + y_0;
		x_test = x_temp;
		i = i + 1;
	}
	
	return i;
}

unsigned char** initImage( int width, int height )
{
	unsigned char** image;
	image = new unsigned char*[width];
	for( int i = 0; i < width; i++ )
	{
		image[i] = new unsigned char[height]; 
	}

	return image;
}

int main( int argc, char** argv )
{
	int index;
	double start_time, end_time;
	int num_tasks, task_id, length;
	char hostname[MPI_MAX_PROCESSOR_NAME];

	MPI_Init( &argc, &argv );
	MPI_Comm_size( MPI_COMM_WORLD, &num_tasks );
	MPI_Comm_rank( MPI_COMM_WORLD, &task_id );
	MPI_Get_processor_name( hostname, &length );

	if( argc < 5 || argc > 5 )
	{
		cout << "Usage: main [width] [height] [iterations] [filename]" << endl;
		MPI_Finalize();
		return 0;
	}

	int width = atoi( argv[1] );
	int height = atoi( argv[2] );
	int iters = atoi( argv[3] );
	string filename = argv[4];
	unsigned char** pixels = initImage( width, height );
	
	if( num_tasks < 2 )
	{
		start_time = MPI_Wtime( );

		for( int i = 0; i < width; i++ )
		{
			for( int j = 0; j < height; j++ )
			{
				index = calc_pixel( i, j, width, height, iters );
				pixels[i][j] = 255 - (index * 10) % 255;
			}
		}

		end_time = MPI_Wtime( );
	}
	else
	{
		if( task_id == 0 )
		{
			start_time = MPI_Wtime( );
			for( int i = 1; i < num_tasks - 1)
			{
				
			}

			end_time = MPI_Wtime( );
		}
		else
		{

		}
	}
	cout << end_time - start_time << endl;

	pim_write_black_and_white( filename.c_str( ), width, height,
							   (const unsigned char**) pixels );

	MPI_Finalize( );
	return 0;
}