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
	for( int i = 0; i < size; i++ )
	{
		if( list[i] == SENT )
		{
			return i;
		}
	}
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
	int index;
	double start_time, end_time;
	int num_tasks, task_id, length;
	char hostname[MPI_MAX_PROCESSOR_NAME];

	MPI_Init( &argc, &argv );
	MPI_Comm_size( MPI_COMM_WORLD, &num_tasks );
	MPI_Comm_rank( MPI_COMM_WORLD, &task_id );
	MPI_Get_processor_name( hostname, &length );

	if( argc < 6 || argc > 6 )
	{
		cout << "Usage: main [width] [height] [iterations] [rows] [filename]" << endl;
		MPI_Finalize();
		return 0;
	}

	int width = atoi( argv[1] );
	int height = atoi( argv[2] );
	int iters = atoi( argv[3] );
	int rows = atoi( argv[4] );
	string filename = argv[5];
	unsigned char** pixels = initImage( width, height );
	
	if( num_tasks < 2 )
	{
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
	}
	else
	{
		if( task_id == 0 )
		{
			uint8_t completed[height];
			int next;
			MPI_Status recv_status;
			int send_buffer[2];
			//int recv_buffer[2];
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
				send_buffer[1] = rows;
				for( int i = 0; i < rows; i++ )
				{
					completed[next+i] = SENT;
				}
				MPI_Send( send_buffer, 2, MPI_INT, i, SENT, MPI_COMM_WORLD );
			}
			while( !shutdown )
			{
				//cout << task_id << endl;
				//MPI_Recv( &recv_buffer, 2, MPI_INT, MPI_ANY_SOURCE, COMPLETE, MPI_COMM_WORLD, &recv_status );
				MPI_Probe( MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &recv_status );
				int source = recv_status.MPI_SOURCE;
				int size;
				MPI_Get_count(&recv_status, MPI_INT, &size);
				int first_row = recv_status.MPI_TAG;
				int num_rows = size / width;

				int return_buffer[width][num_rows];
				MPI_Recv( return_buffer, size, MPI_INT, source, first_row, MPI_COMM_WORLD, &recv_status );
//				printf( "Recv from %d\n", source );
				for( int j = 0; j < num_rows; j++ )
				{
				//	if( !completed[first_row+j] )
			//		{
						for( int i = 0; i < width; i++ )
						{
							pixels[i][first_row+j] = 255 - return_buffer[i][j] * 10 % 255 ;
							//printf( "Pixel: (%d, %d)\n", i, first_row+j );
						}
						completed[first_row+j] = COMPLETE;
			//			printf( "Finished Row: %d\n", first_row+j );
			//		}
				}

//				delImage( return_buffer, width, num_rows );

				if( !isComplete( completed, height ) )
				{
					next = nextRow( completed, height );	
					send_buffer[0] = next;
					if ( send_buffer[0] + rows >= height )
					{
						send_buffer[1] = height - send_buffer[0];
					}
					else
					{
						send_buffer[1] = rows;
					}
					MPI_Send( send_buffer, 2, MPI_INT, source, SENT, MPI_COMM_WORLD );
					for( int i = 0; i < send_buffer[1]; i++ )
					{
						completed[i] = SENT;
					}
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

			while( !shutdown )
			{
				MPI_Probe( MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &recv_status );
				if( recv_status.MPI_TAG == SHUTDOWN )
				{
					shutdown = true;
				}
				else if( recv_status.MPI_TAG == SENT )
				{	
					MPI_Recv( recv_buffer, 2, MPI_INT, 0, SENT, MPI_COMM_WORLD, &recv_status );
					int first_row = recv_buffer[0];
					int num_rows = recv_buffer[1];
					int return_buffer[width][num_rows];

					for( int i = 0; i < width; i++ )
					{
						for( int j = 0; j < num_rows; j++ )
						return_buffer[i][j] = calc_pixel( first_row+j, i, width, height, iters );
					}
				//	MPI_Send( recv_buffer, 2, MPI_INT, 0, COMPLETE, MPI_COMM_WORLD );
					MPI_Send( return_buffer, width * num_rows, MPI_INT, 0, first_row, MPI_COMM_WORLD );
//					printf( "Send from %d\n", task_id );
				}
			}
		//	printf( "Task %d shutdown\n", task_id );
		}
	}
	if( task_id == 0 )
	{
		pim_write_black_and_white( filename.c_str( ), width, height,
							   (const unsigned char**) pixels );
		printf( "%d,%d,%d,%.3f\r\n", num_tasks, width, rows, end_time - start_time );
	}

	delImage( pixels, width, height );
	MPI_Finalize( );
//	printf( "Close Task: %d\n", task_id);

	return 0;
}