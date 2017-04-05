#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <string>
#include <algorithm>
#include <mpi.h>
#include <stdint.h>
using namespace std;

#define NUM_BUCKETS 10
#define MAX_NUMBER 1000
#define INITIAL 1
#define REDISTRIBUTE 2
#define COLLECT 3

vector<int> readFile( string filename );
vector<int> bucketSort( vector<int> input );
void printList( vector<int> input );
void sequential( string filename );
void master_node( string filename, int num_tasks );
void slave_node( string filename, int task_id, int num_tasks );

int main( int argc, char** argv )
{
	int num_tasks, task_id;
	MPI_Init( &argc, &argv );
	MPI_Comm_size( MPI_COMM_WORLD, &num_tasks );
	MPI_Comm_rank( MPI_COMM_WORLD, &task_id );

	if( argc < 2 || argc > 2 )
	{
		cout << "Usage: main [filename]" << endl;
		return 1;
	}

	string filename = argv[1];

	if ( num_tasks == 1 )
	{
		sequential( filename );
	}
	else
	{
		if( task_id == 0 )
		{
			master_node( filename, num_tasks );
		}
		else
		{
			slave_node( filename, task_id, num_tasks );
		}
	}

	MPI_Finalize();
	return 0;
}

vector<int> readFile( string filename )
{
	ifstream inFile( filename );
	uint64_t count;
	vector<int> list;
	int temp;
	if( !inFile )
	{
		return list;
	}
	inFile >> count;
	for( int i = 0; i < count; i++ )
	{
		inFile >> temp;
		list.push_back( temp );
	}

	return list;
}

vector<int> bucketSort( vector<int> input )
{
	uint64_t count = input.size( );
	int bucket;
	array<vector<int>, NUM_BUCKETS> buckets;
	vector<int> output;
	int max_number = *max_element( input.begin( ), input.end( ) );
	int bucket_size = max_number / NUM_BUCKETS+1;

	for( int i = 0; i < count; i++ )
	{
		bucket = input[i] / bucket_size;
		buckets[bucket].push_back( input[i] );	
	}

	for( int i = 0; i < NUM_BUCKETS; i++ )
	{
		sort( buckets[i].begin(), buckets[i].end() );
		for( int j = 0; j < buckets[i].size( ); j++ )
		{
			output.push_back( buckets[i][j] );
		}
	}

	return output;
}

void printList( vector<int> input )
{
	for( int i = 0; i < input.size( ); i++ )
	{
		cout << input[i] << endl;
	}
}

void sequential( string filename )
{
	vector<int> list = readFile( filename );
	if( list.size( ) == 0 )
	{
		cout << "File Open Error" << endl;
		return;
	}

	double start_time = MPI_Wtime( );
	list = bucketSort( list );
	double end_time = MPI_Wtime( );

	cout << list.size( ) << "," << end_time - start_time << endl;
	MPI_Finalize();
	return;
}

void master_node( string filename, int num_tasks )
{
	vector<int> masterList = readFile( filename );
	uint64_t size = masterList.size( );
	if( size == 0 )
	{
		cout << "File Open Error" << endl;
		MPI_Abort( MPI_COMM_WORLD, 2 );
	}
	
	int max_size = *max_element( masterList.begin( ), masterList.end( ) );
	int bucket_width = max_size / num_tasks + 1;
	int big_segment = size / (num_tasks);
//	int small_segment = size - big_segment * ( num_tasks - 1 );
	vector<int> buffer;

	for( int i = 1; i < num_tasks; i++ )
	{
		for( int j = 0; j < big_segment; j++ )
		{
			buffer.push_back( masterList[(i - 1) * big_segment + j] );
		}
		MPI_Send( &buffer[0], buffer.size( ), MPI_INT, i, INITIAL, MPI_COMM_WORLD );
		MPI_Send( &max_size, 1, MPI_INT, i, INITIAL, MPI_COMM_WORLD );
		buffer.clear( );
	}
	vector<vector<int>> buckets;
	for( int i = 0; i < num_tasks; i++ )
	{
		vector<int> temp;
		buckets.push_back( temp );
	}

	double start_time = MPI_Wtime( );

	for(int i = ( num_tasks - 1 ) * big_segment; i < size; i++ )
	{	
		buckets[ masterList[i] / (bucket_width) ].push_back( masterList[i] );
	}
	MPI_Request requests[num_tasks];
	
	for( int i = 1; i < num_tasks; i++ )
	{
		MPI_Isend( &buckets[i][0], buckets[i].size( ), MPI_INT, i, REDISTRIBUTE, MPI_COMM_WORLD, &requests[i] );
	}

	MPI_Status recv_status;
	int recv_count;
	for( int i = 1; i < num_tasks; i++ )
	{
		MPI_Probe( i, REDISTRIBUTE, MPI_COMM_WORLD, &recv_status );
		MPI_Get_count( &recv_status, MPI_INT, &recv_count );
		int* buffer2 = new int[recv_count];
		MPI_Recv( buffer2, recv_count, MPI_INT, i, REDISTRIBUTE, MPI_COMM_WORLD, &recv_status );
		for(int j = 0; j < recv_count; j++ )
		{
			buckets[0].push_back( buffer2[j] );
		}
	}
	
	for( int i = 1; i < num_tasks; i++ )
	{
		MPI_Wait( &requests[i], &recv_status );
	}

	sort( buckets[0].begin( ), buckets[0].end( ) );

	for( int i = 1; i < num_tasks; i++ )
	{
		MPI_Recv( &recv_count, 1, MPI_INT, i, COLLECT, MPI_COMM_WORLD, &recv_status );
	}
	double end_time = MPI_Wtime( );
	cout << num_tasks << "," << size << "," << end_time - start_time << "\r\n";
}

void slave_node( string filename, int task_id, int num_tasks )
{
	MPI_Status recv_status;
	int recv_count, max_value;
	MPI_Probe( 0, INITIAL, MPI_COMM_WORLD, &recv_status );
	MPI_Get_count( &recv_status, MPI_INT, &recv_count );
	int *buffer = new int[recv_count];
	int buffer_size = recv_count;
	MPI_Recv( buffer, recv_count, MPI_INT, 0, INITIAL, MPI_COMM_WORLD, &recv_status );
	MPI_Recv( &max_value, 1, MPI_INT, 0, INITIAL, MPI_COMM_WORLD, &recv_status );
	int bucket_width = max_value / num_tasks + 1;

	vector<vector<int>> buckets;
	for( int i = 0; i < num_tasks; i++ )
	{
		vector<int> temp;
		buckets.push_back( temp );
	}

	for( int i = 0; i < buffer_size; i++ )
	{
		buckets[ buffer[i] / ( bucket_width ) ].push_back( buffer[i] );
	}
	MPI_Request requests[num_tasks];
	for( int i = 0; i < num_tasks; i++ )
	{
		if( i != task_id )
		{
			MPI_Isend( &buckets[i][0], buckets[i].size( ), MPI_INT, i, REDISTRIBUTE, MPI_COMM_WORLD, &requests[i] );
		}
	}
	for( int i = 0; i < num_tasks; i++ )
	{
		if( i != task_id )
		{
			MPI_Probe( i, REDISTRIBUTE, MPI_COMM_WORLD, &recv_status );
			MPI_Get_count( &recv_status, MPI_INT, &recv_count );
			int* buffer2 = new int[recv_count];
			MPI_Recv( buffer2, recv_count, MPI_INT, i, REDISTRIBUTE, MPI_COMM_WORLD, &recv_status );
			for(int j = 0; j < recv_count; j++ )
			{
				buckets[task_id].push_back( buffer2[j] );
			}
		}
	}
	
	for( int i = 0; i < num_tasks; i++ )
	{
		if( i != task_id )
		{
			MPI_Wait( &requests[i], &recv_status );
		}
	}
	sort( buckets[task_id].begin( ), buckets[task_id].end( ) );

	MPI_Send( &recv_count, 1, MPI_INT, 0, COLLECT, MPI_COMM_WORLD );
}