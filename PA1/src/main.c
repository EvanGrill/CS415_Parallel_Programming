#include "mpi.h"
#include <stdlib.h>
#include <stdio.h>

#define MASTER 0
#define MESSAGE_COUNT 1000000

int main( int argc, char** argv )
{
	double start_time, end_time;
	int num_tasks, task_id, length;
	char hostname[MPI_MAX_PROCESSOR_NAME];
	int send_result, recv_result;
	int send_value, recv_value;
	int i;

	MPI_Init( &argc, &argv );
	MPI_Comm_size( MPI_COMM_WORLD, &num_tasks );
	MPI_Comm_rank( MPI_COMM_WORLD, &task_id );
	MPI_Get_processor_name( hostname, &length );

	if( num_tasks < 2 )
	{
		printf( "Insufficient Tasks.\n");
		MPI_Finalize();
		return -1;
	}
//	else
//	{
//		printf( "Task %d reporting in.\n", task_id );
//	}

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

	// close MPI
	MPI_Finalize();
}