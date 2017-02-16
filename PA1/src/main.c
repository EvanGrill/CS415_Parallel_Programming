#include "mpi.h"
#include <stdlib.h>
#include <stdio.h>

#define MASTER 0
#define MESSAGE_COUNT 1000000
#define BUFFER_COUNT 5000

int main( int argc, char** argv )
{
	double start_time, end_time;
	int num_tasks, task_id, length;
	char hostname[MPI_MAX_PROCESSOR_NAME];
	int send_value, recv_value;
	int i;
	int mode;
	int* send_buffer;
	int* recv_buffer;

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
		start_time = MPI_Wtime( );

		for( i = 0; i < MESSAGE_COUNT; i++ )
		{
			send_value = i;
			if( task_id == MASTER )
			{
				MPI_Send( &send_value, 1, MPI_INT, 1, 0, MPI_COMM_WORLD );
				MPI_Recv( &send_value, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
	//			printf( "Message received by: %d\n", task_id );
			}
			else if( task_id == 1 )
			{
				MPI_Recv( &recv_value, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
	//			printf( "Message received by: %d\n", task_id );
				MPI_Send( &recv_value, 1, MPI_INT, 0, 0, MPI_COMM_WORLD );
			}
		
		}

		end_time = MPI_Wtime( );

		if( task_id == MASTER )
		{
			printf( "Messages Sent: %d\n", MESSAGE_COUNT );
			printf( "Total time: %.10f secs\n", (end_time - start_time) );
			printf( "Average Round Trip: %.10f secs\n", (end_time - start_time) / MESSAGE_COUNT );
		}
	}
	else
	{
		FILE* output = fopen( "buffer_test.csv", "w" );

		if( !output )
		{
			printf( "Unable to write to file.\n" );
			MPI_Finalize( );
			return 0;
		}

		fprintf( output, "buffer_size, time\r\n" );

		for( i = 1; i <= BUFFER_COUNT; i++ )
		{
			send_buffer = malloc( sizeof(int) * i );
			recv_buffer = malloc( sizeof(int) * i );
			
			start_time = MPI_Wtime( );

			if( task_id == MASTER )
			{
				MPI_Send( send_buffer, i, MPI_INT, 1, 0, MPI_COMM_WORLD );
			}
			else
			{
				MPI_Recv( recv_buffer, i, MPI_INT, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
			}

			end_time = MPI_Wtime( );

			fprintf( output, "%d,%.10f\r\n", i, ( end_time - start_time ) );
		}

		fclose;
	}

	// close MPI
	MPI_Finalize();
	return 0;
}