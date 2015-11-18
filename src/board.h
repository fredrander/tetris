#ifndef __board_h__
#define __board_h__

#include <stdbool.h>
#include <SDL/SDL.h>
#include "tetris.h"


// handle the game board

// initialize
void board_init();

// clear board
void board_clear();

// draw current board
void board_draw();

typedef enum CollisionWhatDef
{
	// mask indicating what we collide with
	COLLISION_DOWN = 0x01,
	COLLISION_UP = 0x02,
	COLLISION_LEFT = 0x04,
	COLLISION_RIGHT = 0x08,
	COLLISION_TETRIMINO = 0x10
	
} CollisionWhat;


typedef enum CollisionHowDef
{
	// mask indicating what part of a Tetrimino is colliding
	SIDE_DOWN = 0x01,
	SIDE_UP = 0x02,
	SIDE_LEFT = 0x04,
	SIDE_RIGHT = 0x08

} CollisionHow;

// information about collision
typedef struct CollisionInfoStruct
{

	// tells what we collide with, bit mask of CollisionWhat
	unsigned char what;
	// tells how we collide, bit mask of CollisionHow
	unsigned char how;

} CollisionInfo;


// check collision
// collision_info might be NULL
bool board_collision(Tetriminos tetrimino, Rotation rotation, int pos_x, int pos_y, CollisionInfo* collision_info);

// add Tetrimino to the board, returns false if it doesn't fit => game over
bool board_add_tetrimino(Tetriminos tetrimino, Rotation rotation, int pos_x, int pos_y);

// update board - remove lines that are filled
// returns number of cleared lines
int board_update();


#endif // __board_h__

