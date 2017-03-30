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

vector<int> readFile( string filename );
vector<int> bucketSort( vector<int> input );
void printList( vector<int> input );

int main( int argc, char** argv )
{
	MPI_Init( &argc, &argv );
	if( argc < 2 || argc > 2 )
	{
		cout << "Usage: main [filename]" << endl;
		return 1;
	}

	string filename = argv[1];
	vector<int> list = readFile( filename );
	if( list.size( ) == 0 )
	{
		cout << "File Open Error" << endl;
		return 2;
	}

	double start_time = MPI_Wtime( );
	list = bucketSort( list );
	double end_time = MPI_Wtime( );

	cout << list.size( ) << "," << end_time - start_time << endl;
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