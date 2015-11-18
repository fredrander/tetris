#ifndef __ui_h__
#define __ui_h__

#include <stdbool.h>
#include <SDL/SDL.h>
#include "tetris.h"


// initialize UI
bool ui_init();

// shutdown UI
void ui_done();

// access tetrimino
const SDL_Rect* ui_get_tetrimino(Tetriminos tetrimino, Rotation rotation);

// draw tetrimino, position in game area coordinates
void ui_draw_tetrimino(Tetriminos tetrimino, Rotation rotation, int game_pos_x, int game_pos_y);

// clear screen where a tetrimino was
void ui_clear_tetrimino(Tetriminos tetrimino, Rotation rotation, int game_pos_x, int game_pos_y);

// draw a single tetrimino square, position in game area coordinates
void ui_draw_tetrimino_square(Tetriminos tetrimino, int game_pos_x, int game_pos_y);

// clear a single tetrimino square
void ui_clear_tetrimino_square(int game_pos_x, int game_pos_y);

// clear the entire game ares
void ui_clear_game_area();

// update game area
void ui_update_game_area();

// write score text
void ui_draw_score(int score);

// write level text
void ui_draw_level(int level);

// write nb. of lines text
void ui_draw_lines(int lines);

// write game time
void ui_draw_time(unsigned long time);

// write high score
void ui_draw_high_score(int score);

// write next target high score
void ui_draw_high_score_next(int score, int position);

// write next Tetrimino
void ui_draw_next(Tetriminos t);

#endif // __ui_h__

