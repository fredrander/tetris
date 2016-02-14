#include "board.h"
#include "tetris.h"
#include "ui.h"
#include <memory.h>
#include <limits.h>
#include <stdio.h>


// holds current board status
// each position can have a Tetrimino or be empty 
static int board[BOARD_WIDTH][BOARD_HEIGHT];

// categorize squares in Tetriminos
static unsigned char tetriminos_square_orientation[7][4][4];

void board_init()
{
	// clear data
	memset(tetriminos_square_orientation, 0, 7*4*4*sizeof(unsigned char));


	// setup tetriminos_square_orientation
	for (int tetrimino = 0; tetrimino< 7; ++tetrimino)
	{
		for (int rotation= 0; rotation < 4; ++rotation)
		{
			// get tetrimino in this rotation
			const SDL_Rect* tetrimino_squares= ui_get_tetrimino(tetrimino, rotation);

			// find min/max values
			int min_x = INT_MAX;
			int min_y = INT_MAX;
			int max_x = INT_MIN;
			int max_y = INT_MIN;
			for (int square= 0; square < 4; ++square)
			{
				if (tetrimino_squares[square].x < min_x)
				{
					min_x = tetrimino_squares[square].x;
				}
				if (tetrimino_squares[square].x > max_x)
				{
					max_x = tetrimino_squares[square].x;
				}
				if (tetrimino_squares[square].y < min_y)
				{
					min_y = tetrimino_squares[square].y;
				}
				if (tetrimino_squares[square].y > max_y)
				{
					max_y = tetrimino_squares[square].y;
				}
			}

			// set information to each square
			for (int square= 0; square < 4; ++square)
			{
				if (tetrimino_squares[square].x == min_x)
				{
					tetriminos_square_orientation[tetrimino][rotation][square] |= SIDE_LEFT;
				}
				if (tetrimino_squares[square].x == max_x)
				{
					tetriminos_square_orientation[tetrimino][rotation][square] |= SIDE_RIGHT;
				}
				if (tetrimino_squares[square].y == min_y)
				{
					tetriminos_square_orientation[tetrimino][rotation][square] |= SIDE_UP;
				}
				if (tetrimino_squares[square].y == max_y)
				{
					tetriminos_square_orientation[tetrimino][rotation][square] |= SIDE_DOWN;
				}
			}
		}
	}
}

void board_clear()
{
	memset(board, -1, sizeof(int)*BOARD_WIDTH*BOARD_HEIGHT);
}

void board_draw()
{
	for (int y= 0; y< BOARD_HEIGHT; ++y)
	{
		for (int x= 0; x< BOARD_WIDTH; ++x)
		{
			if (board[x][y] >= 0)
			{
				ui_draw_tetrimino_square(board[x][y], x, y);
			}
		}
	}
}

bool board_collision(Tetriminos tetrimino, Rotation rotation, int pos_x, int pos_y, CollisionInfo* collision_info)
{
	if (collision_info != NULL)
	{
		memset(collision_info, 0, sizeof(CollisionInfo));
	}

	// get tetrimino in this rotation
	const SDL_Rect* tetrimino_squares= ui_get_tetrimino(tetrimino, rotation);

	bool found_collision = false;

	// check each square
	for (int i= 0; i< 4; ++i)
	{
		int x = pos_x + tetrimino_squares[i].x;
		int y = pos_y + tetrimino_squares[i].y;
		bool pos_x_ok = false;
		bool pos_y_ok = false;
		unsigned char collision_what = 0;
		
		// check board edges
		if (x < 0)
		{
			collision_what |= COLLISION_LEFT;
		}
		else if (x >= BOARD_WIDTH)
		{
			collision_what |= COLLISION_RIGHT;
		}
		else
		{
			pos_x_ok = true;
		}
		if (y < 0)
		{
			collision_what |= COLLISION_UP;
		}
		else if (y >= BOARD_HEIGHT)
		{
			collision_what |= COLLISION_DOWN;
		}
		else
		{
			pos_y_ok = true;
		}

		// do we have a Tetriminos square at the position
		if (pos_x_ok && pos_y_ok && board[x][y] >= 0)
		{
			// set collision with Tetrimino
			collision_what |= COLLISION_TETRIMINO;
		}

		// set out data
		if (collision_info != NULL && collision_what != 0)
		{
			collision_info->what |= collision_what;
			collision_info->how |= tetriminos_square_orientation[tetrimino][rotation][i];
		}

		if (collision_what != 0)
		{
			found_collision = true;
		}
	}
	
	return found_collision;
}

bool board_add_tetrimino(Tetriminos tetrimino, Rotation rotation, int pos_x, int pos_y)
{
	// get tetrimino in this rotation
	const SDL_Rect* tetrimino_squares= ui_get_tetrimino(tetrimino, rotation);

	// add each square
	for (int i= 0; i< 4; ++i)
	{
		int x = pos_x + tetrimino_squares[i].x;
		int y = pos_y + tetrimino_squares[i].y;
		
		if (x < 0 || x >= BOARD_WIDTH)
		{
			return false;
		}
		if (y < 0 || y >= BOARD_HEIGHT)
		{
			return false;
		}
		if (board[x][y] >= 0)
		{
			return false;
		}
		
		board[x][y] = tetrimino;
	}
	
	return true;
}

void board_remove_line(int line)
{
	// clear line
	for (int x= 0; x < BOARD_WIDTH; ++x)
	{
		board[x][line] = -1;
	}

	// move everything down
	for (int x = 0; x < BOARD_WIDTH; ++x)
	{
		for (int y = line - 1; y >= 0; --y)
		{
			// move down one line
			if (board[x][y] != -1)
			{
				int new_y = y + 1;

				// move block
				board[x][new_y] = board[x][y];
				board[x][y] = -1;
			}
		}
	}
}

int board_update()
{
	// start from the bottom
	// keep on while we remove lines
	bool done = false;
	int removed_lines_cnt = 0;
	while (!done)
	{
		bool line_removed = false;
		for (int y= BOARD_HEIGHT - 1; y >= 0 && !line_removed; --y)
		{
			bool found_empty = false;
			for (int x = 0; x < BOARD_WIDTH && !found_empty; ++x)
			{
				found_empty = (board[x][y] == -1);
			}

			if (!found_empty)
			{
				// a line with no empty squares was found
				// remove it
				line_removed = true;
				++removed_lines_cnt;

				board_remove_line(y);
			}
		}

		done = !line_removed;
	}

	return removed_lines_cnt;
}
