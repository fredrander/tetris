#include "score.h"
#include "ui.h"
#include "highscore.h"
#include <assert.h>
#include <limits.h>


#define SCORE_UPDATE_INTERVAL 500


static int score = 0;
static int level = 1;
static int lines = 0;
static unsigned long start_time = 0;
static unsigned long stop_time = 0;
static int prev_score_draw = 0;
static int prev_level_draw = 0;
static int prev_lines_draw = 0;
static unsigned long prev_time_draw = 0;
static int line_clear_value = 0;
static int last_level_clear_limit = 0;
static unsigned long score_draw_clock = 0;

// drop speed for level
static unsigned long drop_speed[30] =
{
	800, // 1
	717, // 2
	633, // 3
	550, // 4
	467, // 5
	383, // 6
	300, // 7
	217, // 8
	133, // 9
	100, // 10
	83, // 11
	83, // 12
	83, // 13
	67, // 14
	67, // 15
	67, // 16
	50, // 17
	50, // 18
	50, // 19
	33, // 20
	33, // 21
	33, // 22
	33, // 23
	33, // 24
	33, // 25
	33, // 26
	33, // 27
	33, // 28
	33, // 29
	17 // 30
};

void score_reset()
{
	score = 0;
	level = 1;
	lines = 0;
	start_time = SDL_GetTicks();
	stop_time = start_time;
	line_clear_value = 0;
	last_level_clear_limit = 0;
	// force repaint
	prev_score_draw = -1;
	prev_level_draw = -1;
	prev_lines_draw = -1;
	prev_time_draw = ULONG_MAX;
	score_draw_clock = SDL_GetTicks();

	high_score_new_game();
}

void score_draw()
{
	// do not render text too often
	unsigned long now = SDL_GetTicks();
	if (now >= score_draw_clock)
	{

		// only draw if needed
		if (score != prev_score_draw)
		{
			ui_draw_score(score);
			prev_score_draw = score;
		}
		if (level != prev_level_draw)
		{
			ui_draw_level(level);
			prev_level_draw = level;
		}
		if (lines != prev_lines_draw)
		{
			ui_draw_lines(lines);
			prev_lines_draw = lines;
		}

		// update time
		unsigned long time = 0;
		if (stop_time > start_time)
		{
			// timer has been stopped
			time = (stop_time - start_time) / 1000ul;
		}
		else
		{
			time = (now - start_time) / 1000ul;
		}
		if (time != prev_time_draw)
		{
			ui_draw_time(time);
			prev_time_draw = time;
		}

		// draw high scores
		high_score_draw(score, (int) time, level, lines);

		score_draw_clock = now + SCORE_UPDATE_INTERVAL;
	}
}

void score_stop_timer()
{
	stop_time = SDL_GetTicks();
}

bool score_game_over()
{
	// update time
	unsigned long now = SDL_GetTicks();
	unsigned long time = 0;
	if (stop_time > start_time)
	{
		// timer has been stopped
		time = (stop_time - start_time) / 1000ul;
	}
	else
	{
		time = (now - start_time) / 1000ul;
	}

	return high_score_check(score, (int) time, level, lines);
}

void score_start_timer()
{
	// calculate how long we were stopped and add that time to start value
	unsigned long now = SDL_GetTicks();
	if (stop_time > start_time && stop_time < now)
	{
		start_time += now - stop_time;
	}

	stop_time = start_time;
}

unsigned long score_get_drop_speed()
{
	int index = level - 1;
	if (index >= 30)
	{
		index = 29;
	}

	return drop_speed[index] * 2;
}

bool score_add_clear(int count)
{
	// 1, 3, 5, 8 pattern
	static int score_pattern[] = { 1, 3, 5, 8 };

	assert(count > 0 && count <= 4);

	bool result = false;

	score += score_pattern[count-1] * 100 * level;
	line_clear_value += score_pattern[count-1];

	// advance level after clearing 5 * level lines
	if (line_clear_value >= (last_level_clear_limit+5*level))
	{
		last_level_clear_limit += 5*level;
		++level;

		result = true;
	}

	lines += count;

	return result;
}

void score_add_soft_drop(int count)
{
	score += count;
}

void score_add_hard_drop(int count)
{
	score += 2*count;
}

void score_add_combo(int count)
{
	score += 50*count*level;
}
