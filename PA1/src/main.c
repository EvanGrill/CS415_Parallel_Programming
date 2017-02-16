#include "mpi.h"
#include <stdlib.h>
#include <stdio.h>

#define MASTER 0
#define MESSAGE_COUNT 1000000
#define MAX_BUFFER 5000
#define BUFFER_COUNT 1000

int main( int argc, char** argv )
{
	double start_time, end_time, sum_time;
	int num_tasks, task_id, length;
	char hostname[MPI_MAX_PROCESSOR_NAME];
	int send_value, recv_value;
	int i, j;
	int mode;
	int send_buffer[MAX_BUFFER];
	int recv_buffer[MAX_BUFFER];

	MPI_Init( &argc, &argv );
	MPI_Comm_size( MPI_COMM_WORLD, &num_tasks );
	MPI_Comm_rank( MPI_COMM_WORLD, &task_id );
	MPI_Get_processor_name( hostname, &length );

	if( num_tasks < 2 )
	{
		printf( "Insufficient Tasks.\n");
		MPI_Finalize();
		return 0;
	}
//	else
//	{
//		printf( "Task %d reporting in.\n", task_id );
//	}
	if( argc < 2 )
	{
		mode = 1;
	}
	else if( argc == 2 )
	{
		mode = atoi(argv[1]);
	}
	if ( argc > 2 || mode < 1 || mode > 2 )
	{
		printf( "Usage: main [1|2]\n");
		printf( "1: Standard Timing (results depend on srun arguments)\n");
		printf( "2: Buffer Size Timing (determine deviation point for buffer size)\n");
		MPI_Finalize();
		return 0;
	}

	if( mode == 1 )
	{
		sum_time = 0.0;
		for( i = 0; i < MESSAGE_COUNT; i++ )
		{
			send_value = i;
			if( task_id == MASTER )
			{
				start_time = MPI_Wtime( );
				MPI_Send( &send_value, 1, MPI_INT, 1, 0, MPI_COMM_WORLD );
				MPI_Recv( &send_value, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
				end_time = MPI_Wtime( );
				sum_time += end_time - start_time;
			}
			else if( task_id == 1 )
			{
				MPI_Recv( &recv_value, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
				MPI_Send( &recv_value, 1, MPI_INT, 0, 0, MPI_COMM_WORLD );
			}
		
		}

		

		if( task_id == MASTER )
		{
			printf( "Messages Sent: %d\n", MESSAGE_COUNT );
			printf( "Total time: %.10f secs\n", sum_time );
			printf( "Average Round Trip: %.10f secs\n", sum_time / MESSAGE_COUNT );
		}
	}
	else
	{
		FILE* output;
		if( task_id == MASTER )
		{
			output = fopen( "buffer_test.csv", "w" );

			if( !output )
			{
				printf( "Unable to write to file.\n" );
				MPI_Finalize( );
				return 0;
			}

			fprintf( output, "buffer_size, time\r\n" );
		}

		for( i = 1; i <= MAX_BUFFER; i++ )
		{
			if( task_id == MASTER )
			{
				sum_time = 0.0;
			}
			for( j=0; j < BUFFER_COUNT; j++ )
			{
				if( task_id == MASTER )
				{
					start_time = MPI_Wtime( );
					MPI_Send( send_buffer, i, MPI_INT, 1, 0, MPI_COMM_WORLD );
					MPI_Recv( send_buffer, i, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
					end_time = MPI_Wtime( );
					sum_time += end_time - start_time;
				}
				else
				{
					MPI_Recv( recv_buffer, i, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
					MPI_Send( send_buffer, i, MPI_INT, 0, 0, MPI_COMM_WORLD );
				}
			}
			if( task_id == MASTER )
			{
				fprintf( output, "%d,%.10f\r\n", i, sum_time / BUFFER_COUNT );
			}
		}

		fclose( output );
	}

	// close MPI
	MPI_Finalize();
	return 0;
}