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
void sequential( uint64_t size );
void master_node( uint64_t size, int num_tasks );
void slave_node( uint64_t size, int task_id, int num_tasks );
int* generateRandoms( uint64_t amount, int maximum );

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

	uint64_t size = strtoull(argv[1], NULL, 0);

	if ( num_tasks == 1 )
	{
		sequential( size );
	}
	else
	{
		if( task_id == 0 )
		{
			master_node( size, num_tasks );
		}
		else
		{
			slave_node( size, task_id, num_tasks );
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

void bucketSort( int* list, uint64_t size )
{
	int bucket;
	array<vector<int>, NUM_BUCKETS> buckets;
	vector<int> output;
	int bucket_size = MAX_NUMBER / NUM_BUCKETS;

	for( int i = 0; i < size; i++ )
	{
		bucket = list[i] / bucket_size;
		buckets[bucket].push_back( list[i] );	
	}

	for( int i = 0; i < NUM_BUCKETS; i++ )
	{
		sort( buckets[i].begin(), buckets[i].end() );
	}

	return;
}

void printList( vector<int> input )
{
	for( int i = 0; i < input.size( ); i++ )
	{
		cout << input[i] << endl;
	}
}

void sequential( uint64_t size )
{
	int* list = generateRandoms( size, MAX_NUMBER );

	double start_time = MPI_Wtime( );
	bucketSort( list, size );
	double end_time = MPI_Wtime( );

	cout << "1," << size << "," << end_time - start_time << endl;
	delete[] list;
	return;
}

void master_node( uint64_t size, int num_tasks )
{
	int* buffer;
	int bucket_width = MAX_NUMBER / num_tasks + 1;
	int big_segment = size / (num_tasks);

	for( int i = 1; i < num_tasks; i++ )
	{
		buffer = generateRandoms( big_segment, MAX_NUMBER );
		MPI_Send( &buffer[0], big_segment, MPI_INT, i, INITIAL, MPI_COMM_WORLD );
		delete[] buffer;
	}

	buffer = generateRandoms( size - big_segment * ( num_tasks - 1 ), MAX_NUMBER );

	vector<vector<int>> buckets;
	for( int i = 0; i < num_tasks; i++ )
	{
		vector<int> temp;
		buckets.push_back( temp );
	}
	
	double start_time = MPI_Wtime( );

	for(int i = 0; i < size - big_segment * ( num_tasks - 1 ); i++ )
	{	
		buckets[ buffer[i] / bucket_width ].push_back( buffer[i] );
	}
	MPI_Request requests[num_tasks];

	delete[] buffer;
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
		buffer = new int[recv_count];
		MPI_Recv( buffer, recv_count, MPI_INT, i, REDISTRIBUTE, MPI_COMM_WORLD, &recv_status );
		for(int j = 0; j < recv_count; j++ )
		{
			buckets[0].push_back( buffer[j] );
		}
		delete[] buffer;
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

void slave_node( uint64_t size, int task_id, int num_tasks )
{
	MPI_Status recv_status;
	int recv_count;
	MPI_Probe( 0, INITIAL, MPI_COMM_WORLD, &recv_status );
	MPI_Get_count( &recv_status, MPI_INT, &recv_count );
	int *buffer = new int[recv_count];
	int buffer_size = recv_count;
	MPI_Recv( buffer, recv_count, MPI_INT, 0, INITIAL, MPI_COMM_WORLD, &recv_status );
	int bucket_width = MAX_NUMBER / num_tasks + 1;

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
			delete[] buffer2;
		}
	}
	delete[] buffer;
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

int* generateRandoms( uint64_t amount, int maximum )
{
	uint64_t i;
	int* list = new int[amount];
	for(i=0;i<amount;i++){
		list[i] = random() % maximum;
	}

	return list;
}