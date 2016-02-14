#include  "tetris.h"

#include <assert.h>


char tetris_tetrimino_name( Tetriminos t )
{
	switch ( t ) 
	{
		case I:
			return 'I';
		case J:
			return 'J';
		case L:
			return 'L';
		case O:
			return 'O';
		case S:
			return 'S';
		case T:
			return 'T';
		case Z:
			return 'Z';
		default:
			assert( 0 );
			return 0;

	};
}

