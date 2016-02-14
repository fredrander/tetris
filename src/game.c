#include "game.h"
#include "ui.h"
#include "board.h"
#include "score.h"
#include <SDL/SDL.h>
#include <assert.h>


// game update interval (fps)
#define UPDATE_INTERVAL 17

// keyboard/joystick repeat interval
#define INPUT_REPEAT_DELAY 125
#define INPUT_REPEAT_INTERVAL 17

// speed up Tetriminos with this factor during soft drop
#define SOFT_DROP_SPEED (falling_speed / 2 > 50 ? 50 : falling_speed / 2)

#define LINE_CLEAR_DELAY 333

// tetrimino in motion
typedef struct FallingTetriminoDef
{

	Tetriminos tetrimino;
	Rotation rotation;
	
	// position
	int x;
	int y;

	unsigned long next_fall;

	bool soft_drop;
	
} FallingTetrimino;

// current falling tetrimino
static FallingTetrimino falling;

// next
static Tetriminos next_tetrimino;

// current falling speed
static unsigned long falling_speed;

// combo count
static int combo_count = 0;

// game over flag
static bool game_over = false;

// user quit
static bool quit = false;

// pause game
static bool pause = false;

// true if last fall resulted in a line clear
static bool line_clear = false;


// create new falling tetrimino
void game_new_falling()
{
	static const char start_position_x[7] = { 3, 3, 3, 4, 3, 3, 3 };

	falling.tetrimino = next_tetrimino;
	falling.rotation = ROTATION_0;

	// create and show next Tetrimino
	next_tetrimino = (Tetriminos) (rand() % 7);
	ui_draw_next(next_tetrimino);

	falling.x = start_position_x[falling.tetrimino];
	falling.y = 0;

	falling.next_fall = SDL_GetTicks() + falling_speed;

	if (line_clear)
	{
		// if we cleared a line we should delay the new Tetrimino (20/60 sec.)
		falling.next_fall += LINE_CLEAR_DELAY; // TODO: Line clear animation
		line_clear = false;
	}

	falling.soft_drop = false;

	printf( "Start %c, next %c\n", tetris_tetrimino_name( falling.tetrimino ),
	     tetris_tetrimino_name( next_tetrimino ) );
}


// draw falling tetrimino at it's current position
void game_draw_falling()
{
	ui_draw_tetrimino(falling.tetrimino, falling.rotation, falling.x, falling.y);
}

// clear falling tetrimino at it's current position
void game_clear_falling()
{
	ui_clear_tetrimino(falling.tetrimino, falling.rotation, falling.x, falling.y);
}

void game_init()
{
	// seed random generator
	srand( SDL_GetTicks() );

	SDL_EnableKeyRepeat(INPUT_REPEAT_DELAY, INPUT_REPEAT_INTERVAL);
}

void game_start()
{
	printf( "New game\n" );

	game_over = false;
	quit = false;
	pause = false;
	
	// reset score
	score_reset();
	combo_count = 0;

	line_clear = false;

	// set start falling speed
	falling_speed = score_get_drop_speed();

	// create first Tetrimino
	next_tetrimino = (Tetriminos) (rand() % 7);

	// clear board
	board_clear();

	// create falling tetrimino and redraw
	game_new_falling();
	game_draw_falling();
	ui_update_game_area();
}

void game_rotate_falling()
{
	// save old rotation
	Rotation old_rotation = falling.rotation;

	falling.rotation++;
	if (falling.rotation > ROTATION_270)
	{
		falling.rotation = ROTATION_0;
	}	

	// move until no collision
	typedef enum MoveDirectionsDef
	{
		LEFT,
		RIGHT,
		UP,
		UNDEFINED
	} MoveDirections;

	int x = falling.x;
	int y = falling.y;
	bool collision = true;
	bool fail = false;
	int step = 1;
	MoveDirections direction = UNDEFINED;
	MoveDirections start_direction = UNDEFINED;
	CollisionInfo collision_info;
	while (collision && !fail)
	{
		collision = board_collision(falling.tetrimino, falling.rotation, x, y, &collision_info);
		if (collision)
		{
			// determine what direction to start moving in
			if (direction == UNDEFINED)
			{
				if ((collision_info.how & SIDE_DOWN) != 0)
				{
					direction = UP;
				}
				else if ((collision_info.how & SIDE_LEFT) != 0)
				{
					direction = RIGHT;
				}
				else
				{
					direction = LEFT;
				}
				start_direction = direction;
			}

			// move in current direction
			switch (direction)
			{
			case UP:
				x = falling.x;
				y = falling.y - step;
				direction = LEFT;
				break;
			case LEFT:
				x = falling.x - step;
				y = falling.y;
				direction = RIGHT;
				break;
			case RIGHT:
				x = falling.x + step;
				y = falling.y;
				direction = UP;
				break;
			case UNDEFINED:
				assert(0);
				break;
			}

			if (direction == start_direction)
			{
				++step;
			}

			if ((collision_info.what & COLLISION_UP) == COLLISION_UP)
			{
				// allow collision on top
				collision = false;
				x = falling.x;
				y = falling.y;
			}
			else if (step > 2) // FIXME: Check width of Tetrimino
			{
				fail = true;
			}
		}
	}

	// restore position if we failed to fit the rotation
	if (fail)
	{
		falling.rotation = old_rotation;
	}
	else
	{
		// update position (if we had to move to fit rotation)
		falling.x = x;
		falling.y = y;
	}
}

void game_move_falling(int x_step)
{
	falling.x += x_step;
	
	CollisionInfo collision_info;
	
	// check collision
	if (board_collision(falling.tetrimino, falling.rotation, falling.x, falling.y, &collision_info))
	{
		// if collides with anything undo the movement
		if ((collision_info.what & (COLLISION_LEFT | COLLISION_RIGHT | COLLISION_TETRIMINO)) != 0)
		{
			falling.x -= x_step;
		}
	}
}

void game_drop_falling()
{
	int old_y = falling.y;

	++falling.y;

	// drop until collision
	while (!board_collision(falling.tetrimino, falling.rotation, falling.x, falling.y, NULL))
	{
		// if collides - return false
		++falling.y;
	}

	// move up since we have gone too far
	--falling.y;

	// award score
	if (falling.y > old_y)
	{
		score_add_hard_drop(falling.y - old_y);
	}

	// force next Tetrimino
	falling.next_fall = SDL_GetTicks();
}

void game_soft_drop_falling()
{
	if (falling.soft_drop)
	{
		// already dropping
		return;
	}

	// set flag and speed up falling Tetrimino
	falling.soft_drop = true;

	unsigned long now = SDL_GetTicks();
	if (now < falling.next_fall)
	{
		if (falling.next_fall - now > SOFT_DROP_SPEED)
		{
			falling.next_fall = now + SOFT_DROP_SPEED;
		}
	}
}

void game_stop_soft_drop_falling()
{
	falling.soft_drop = false;
}

void game_handle_event()
{
	SDL_Event event;
	
	// check if event occured
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_KEYDOWN:

				if (!pause && !game_over)
				{
					switch (event.key.keysym.sym)
					{
						case SDLK_UP:
							game_rotate_falling();
							break;
						case SDLK_LEFT:
							game_move_falling(-1);
							break;
						case SDLK_RIGHT:
							game_move_falling(+1);
							break;
						case SDLK_DOWN:
							game_drop_falling();
							break;
						case SDLK_SPACE:
							game_soft_drop_falling();
							break;
						case SDLK_ESCAPE:
						case SDLK_q:
							quit = true;
							break;
						case SDLK_PAUSE:
						case SDLK_p:
							pause = true;
							score_stop_timer();
							printf( "Pause\n" );
							break;
						default:
							break;
					}
				}
				else
				{
					switch (event.key.keysym.sym)
					{
						case SDLK_ESCAPE:
						case SDLK_q:
							quit = true;
							break;
						case SDLK_PAUSE:
						case SDLK_p:
							if (pause)
							{
								pause = false;
								falling.next_fall = SDL_GetTicks() + falling_speed;
								score_start_timer();
								printf( "Continue\n" );
							}
							break;
						case SDLK_n:
							game_start();
							break;
						default:
							// NOP
							break;
					}
				}
				break;

			case SDL_KEYUP:

				if (!pause && !game_over)
				{
					switch (event.key.keysym.sym)
					{
					case SDLK_SPACE:
						game_stop_soft_drop_falling();
						break;
					default:
						// NOP
						break;
					}
				}
				break;
				
			default:
				break;
		}
	}
}

// wait until next update
void game_sleep_until_next_update()
{
	static unsigned long next_update = 0;
	unsigned long now;
	
	now = SDL_GetTicks();
	
	if (next_update <= now)
	{
		next_update = now + UPDATE_INTERVAL;
		return;
	}

	SDL_Delay(next_update - now);
	next_update += UPDATE_INTERVAL;
}

bool game_update_falling()
{
	unsigned long now = SDL_GetTicks();
	unsigned long speed = falling_speed;

	if (falling.soft_drop)
	{
		speed = SOFT_DROP_SPEED;
	}
	
	if (falling.next_fall <= now)
	{
		falling.next_fall = falling.next_fall + speed;
		++falling.y;
		
		// check collision
		if (board_collision(falling.tetrimino, falling.rotation, falling.x, falling.y, NULL))
		{
			// if collides - return false
			--falling.y;
			return false;
		}
		else
		{
			// award soft drop
			if (falling.soft_drop)
			{
				score_add_soft_drop(1);
			}
		}
	}
	
	return true;
}

bool game_update_board()
{
	if (!board_add_tetrimino(falling.tetrimino, falling.rotation, falling.x, falling.y))
	{
		return false;
	}

	int cleared = board_update();
	if (cleared > 0)
	{
		line_clear = true;

		printf( "Cleared %d line(s)\n", cleared );

		if (score_add_clear(cleared))
		{
			// set new speed if we increased level
			falling_speed = score_get_drop_speed();
		}
		combo_count++;
		if (combo_count > 1)
		{
			printf( "Combo %d\n", combo_count );

			score_add_combo(combo_count-1);
		}
	}
	else
	{
		// reset combo count
		combo_count = 0;
	}

	return true;
}

bool game_update()
{
	// wait until it's time to update
	game_sleep_until_next_update();

	// clear old tetrimino position
	game_clear_falling();

	// handle keyboard events
	game_handle_event();
	if (quit)
	{
		return false;
	}
	
	// update y-position, start new tetrimino if at the end
	if (!pause && !game_over)
	{
		if (!game_update_falling())
		{
			// add the Tetrimino to the board
			if (!game_update_board())
			{
				game_over = true;
				score_stop_timer();
				score_game_over();

				printf( "Game over\n" );
			}
			else
			{
				game_new_falling();
			}
		}
	}

	// repaint
	ui_clear_game_area();
	board_draw();
	game_draw_falling();
	ui_update_game_area();
	score_draw();
	
	return true;
}

