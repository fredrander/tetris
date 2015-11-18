#ifndef __tetris_h__
#define __tetris_h__

// some general tetris definitions

// the different pieces, or Tetriminos
typedef enum TetriminosDefs
{
	I,
	J,
	L,
	O,
	S,
	T,
	Z
} Tetriminos;

// rotation angles
typedef enum RotationDefs
{
	ROTATION_0,
	ROTATION_90,
	ROTATION_180,
	ROTATION_270
} Rotation;

// size of game board
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20


#endif // __tetris_h___

