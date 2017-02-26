#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

#include "PIMFuncs.hpp"

#define MAX_ITERATION 1000
#define WIDTH 640
#define HEIGHT 480
#define FILENAME "output.ppm"

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

uint8_t calc_pixel( int x, int y, int width, int height )
{
	double x_0 = ( 4.0 * x ) / width - 2.0;
	double y_0 = ( 4.0 * y ) / height - 2.0;
	double x_test = 0.0;
	double y_test = 0.0;
	int iteration = 0;
	double x_temp;
	while( x_test * x_test + y_test * y_test < 4 &&
		   iteration < MAX_ITERATION )
	{
		x_temp = x_test * x_test - y_test * y_test + x_0;
		y_test = 2 * x_test * y_test + y_0;
		x_test = x_temp;
		iteration = iteration + 1;
	}
	
	return iteration;
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
	vector<Color> colorList = readColors( "colors.txt" );
	if( colorList.size( ) == 0)
	{
		cout << "Color list not read." << endl;
		return 1;
	}

/*	unsigned char** red = initImage( WIDTH, HEIGHT );
	unsigned char** blue = initImage( WIDTH, HEIGHT );
	unsigned char** green = initImage( WIDTH, HEIGHT );
*/  unsigned char** pixels = initImage( WIDTH, HEIGHT );

	int index;

	for( int i = 0; i < WIDTH; i++ )
	{
		for( int j = 0; j < HEIGHT; j++ )
		{
			index = calc_pixel( j, i , WIDTH, HEIGHT );
/*			red[i][j] = (unsigned char) colorList[index].red;
			blue[i][j] = (unsigned char) colorList[index].blue;
			green[i][j] = (unsigned char) colorList[index].green;
			
*/			pixels[i][j] = 255 - (index * 5) % 255;
		}
	}

//	pim_write_color( FILENAME, WIDTH, HEIGHT,
//					 (const unsigned char**) red,
//					 (const unsigned char**) blue,
//					 (const unsigned char**) green );

	pim_write_black_and_white( FILENAME, WIDTH, HEIGHT,
							   (const unsigned char**) pixels );
	return 0;
}