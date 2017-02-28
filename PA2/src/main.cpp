#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <mpi.h>
using namespace std;

#include "PIMFuncs.hpp"

#define SHUTDOWN 3
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

bool isComplete( uint8_t list[], int size )
{
	for( int i = 0; i < size; i++ )
	{
		if( list[i] != COMPLETE )
		{
			return false;
		}
	}

	return true;
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
	for( int i = 0; i < size; i++ )
	{
		if( list[i] == SENT )
		{
			return i;
		}
	}
	return -1;
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
	
	if( num_tasks == 1 )
	{
		cout << "Sequential Mode" << endl;
		start_time = MPI_Wtime( );

		for( int i = 0; i < width; i++ )
		{
			for( int j = 0; j < height; j++ )
			{
				index = calc_pixel( j, i, width, height, iters );
				pixels[i][j] = 255 - (index * 10) % 255;
			}
		}

		end_time = MPI_Wtime( );
		cout << end_time - start_time << endl;
	}
	else
	{
		if( task_id == 0 )
		{
			cout << "Task: " << task_id << "Parallel Mode" << endl;
			uint8_t completed[height];
			int next;
			MPI_Status recv_status;
			int send_buffer[2];
			int recv_buffer[2];
			int* return_buffer;
			bool shutdown = false;

			for( int i = 0; i < height; i++ )
			{
				completed[i] = NONE;
			}

			start_time = MPI_Wtime( );
			for( int i = 1; i < num_tasks ; i++ )
			{
				next = nextRow( completed, height );	
				send_buffer[0] = next;
				send_buffer[1] = 1;
				completed[next] = SENT;
				MPI_Send( send_buffer, 2, MPI_INT, i, SENT, MPI_COMM_WORLD );
			}
			while( !shutdown )
			{
				//cout << task_id << endl;
				MPI_Recv( &recv_buffer, 2, MPI_INT, MPI_ANY_SOURCE, COMPLETE, MPI_COMM_WORLD, &recv_status );
				int source = recv_status.MPI_SOURCE;
				int first_row = recv_buffer[0];
//				int num_rows = recv_buffer[1];

				return_buffer = new int[width];
				MPI_Recv( return_buffer, width, MPI_INT, source, COMPLETE, MPI_COMM_WORLD, &recv_status );

				if( completed[first_row] != COMPLETE )
				{
					for( int i = 0; i < width; i++ )
					{
						pixels[i][first_row] = 255 - return_buffer[i] * 10 % 255 ;
					//	printf( "%d ", pixels[i][first_row] );
					}
					completed[first_row] = COMPLETE;
				}
				delete return_buffer;

				if( !isComplete( completed, height ) )
				{
					next = nextRow( completed, height );	
					send_buffer[0] = next;
					send_buffer[1] = 1;
					completed[next] = SENT;
					MPI_Send( send_buffer, 2, MPI_INT, source, SENT, MPI_COMM_WORLD );
				}
				else
				{
					for( int i = 1; i < num_tasks; i++ )
					{
						MPI_Send( send_buffer, 2, MPI_INT, i, SHUTDOWN, MPI_COMM_WORLD );
					}
					shutdown = true;
				}
			}

			end_time = MPI_Wtime( );
		}
		else
		{
			bool shutdown = false;
			MPI_Status recv_status;
			int recv_buffer[2];
			int* return_buffer;

			while( !shutdown )
			{
				MPI_Recv( recv_buffer, 2, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &recv_status );
				if( recv_status.MPI_TAG == SHUTDOWN )
				{
					shutdown = true;
				}
				else
				{	
					int first_row = recv_buffer[0];
//					int num_rows = recv_buffer[1];
					return_buffer = new int[width];

					for( int i = 0; i < width; i++ )
					{
						return_buffer[i] = calc_pixel( first_row, i, width, height, iters );
					}
					MPI_Send( recv_buffer, 2, MPI_INT, 0, COMPLETE, MPI_COMM_WORLD );
					MPI_Send( return_buffer, width, MPI_INT, 0, COMPLETE, MPI_COMM_WORLD );

					delete return_buffer;
				}
			}
		}
	}
	if( task_id == 0 )
	{
		pim_write_black_and_white( filename.c_str( ), width, height,
							   (const unsigned char**) pixels );
		cout << end_time - start_time << endl;
	}

	MPI_Finalize( );
	return 0;
}