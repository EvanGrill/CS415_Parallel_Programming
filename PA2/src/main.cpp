#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <mpi.h>
using namespace std;

#include "PIMFuncs.hpp"

#define SHUTDOWN 500000
#define NONE 0
#define SENT 1
#define COMPLETE 2

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

int **alloc_2d_int(int rows, int cols) {
    int *data = (int *)malloc(rows*cols*sizeof(int));
    int **array= (int **)malloc(rows*sizeof(int*));
    for (int i=0; i<rows; i++)
        array[i] = &(data[cols*i]);

    return array;
}

void delImage( unsigned char** image, int width, int height )
{
	for( int i = 0; i < width; i++ )
	{
		delete[] image[i];
	}
	delete[] image;
}

int nextRow( uint8_t list[], int size )
{
	for( int i = 0; i < size; i++ )
	{
		if( list[i] == NONE )
		{
			return i;
		}
	}
/*	for( int i = 0; i < size; i++ )
	{
		if( list[i] == SENT )
		{
			return i;
		}
	} */
	return -1;
}

bool isComplete( uint8_t list[], int size )
{
/*	for( int i = 0; i < size; i++ )
	{
		if( list[i] != COMPLETE )
		{
			return false;
		}
	}

	return true; */
	return nextRow( list, size ) == -1;
}

int main( int argc, char** argv )
{
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
	// int rows = atoi( argv[4] );
	string filename = argv[4];
	unsigned char** pixels = initImage( width, height );
	
	if( num_tasks < 2 )
	{
		int index;
		start_time = MPI_Wtime( );

		for( int i = 0; i < width; i++ )
		{
			for( int j = 0; j < height; j++ )
			{
				index = calc_pixel( j, i, width, height, iters );
				pixels[i][j] = (unsigned char) ( 255 - (index * 10) % 255 );
			}
		}

		end_time = MPI_Wtime( );
	}
	else
	{
		int rows = height / (num_tasks - 1);
		if( task_id == 0 )
		{
			int next;
			MPI_Status recv_status;
			int send_buffer[2];
			//int recv_buffer[2];
			bool shutdown = false;
			bool completed[num_tasks];

			start_time = MPI_Wtime( );
			completed[0] = true;

			for( int i = 1; i < num_tasks ; i++ )
			{
				next = ( i - 1 ) * rows;	
				send_buffer[0] = next;
				if ( next + rows >= height )
				{
					send_buffer[1] = height - next;
				}
				else
				{
					send_buffer[1] = rows;
				}
				MPI_Send( send_buffer, 2, MPI_INT, i, SENT, MPI_COMM_WORLD );
				completed[i] = false;
			}
			while( !shutdown )
			{
				MPI_Probe( MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &recv_status );
				int source = recv_status.MPI_SOURCE;
				int size;
				MPI_Get_count(&recv_status, MPI_INT, &size);
				int first_row = recv_status.MPI_TAG;
				int num_rows = size / height;

				int** return_buffer = alloc_2d_int( width, num_rows );
				MPI_Recv( &(return_buffer[0][0]), size, MPI_INT, source, first_row, MPI_COMM_WORLD, &recv_status );
				for( int j = 0; j < num_rows; j++ )
				{
						for( int i = 0; i < width; i++ )
						{
							pixels[i][first_row+j] = (unsigned char) ( 255 - return_buffer[i][j] * 10 % 255 );
						}
				}
				completed[source] = true;
				
				shutdown = true;
				for( int i = 0; i < num_tasks - 1; i++ )
				{
					if( completed[i] == false )
					{
						shutdown = false;
					}
				}
			}
			end_time = MPI_Wtime( );	
		}
		else
		{
			MPI_Status recv_status;
			int recv_buffer[2];
			MPI_Recv( recv_buffer, 2, MPI_INT, 0, SENT, MPI_COMM_WORLD, &recv_status );
			int first_row = recv_buffer[0];
			int num_rows = recv_buffer[1];

			int** return_buffer = alloc_2d_int( width, num_rows );

			for( int i = 0; i < width; i++ )
			{
				for( int j = 0; j < num_rows; j++ )
				{	
					return_buffer[i][j] = calc_pixel( first_row + j, i, width, height, iters );
				//	cout << task_id << ": " << i << " " << j << endl;
				}
			}
			MPI_Send( &(return_buffer[0][0]), width * num_rows, MPI_INT, 0, first_row, MPI_COMM_WORLD );
		}
	}
	if( task_id == 0 )
	{
		pim_write_black_and_white( filename.c_str( ), width, height,
							   (const unsigned char**) pixels );
		printf( "%d,%d,%.3f\r\n", num_tasks, width, end_time - start_time );
		delImage( pixels, width, height );
	}
	MPI_Finalize( );
//	MPI_Abort( MPI_COMM_WORLD, 0 );

	return 0;
}