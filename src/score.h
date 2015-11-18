#ifndef __score_h__
#define __score_h__

#include <stdbool.h>


// reset current score
void score_reset();

// draw current score
void score_draw();

// start/stop the timer
void score_stop_timer();
void score_start_timer();

// tell scoring manager that game is over, will add to highscore
// returen true if highscore
bool score_game_over();

// get current drop speed
unsigned long score_get_drop_speed();

// clear lines, returns true if new level
bool score_add_clear(int count);

// soft drop
void score_add_soft_drop(int count);

// hard drop
void score_add_hard_drop(int count);

// combo
void score_add_combo(int count);

// TODO: T-Spin

#endif // __score_h__
