#include "highscore.h"
#include "ui.h"
#include <stdio.h>
#include <stdbool.h>


#define FILE_NAME ".highscores"
#define NUMBER_OF_HIGH_SCORES 10

typedef struct HighScoreStruct
{
	int score;
	int time;
	int level;
	int lines;

} HighScore;

static HighScore high_scores[NUMBER_OF_HIGH_SCORES];
static bool dirty = true;
static int prev_draw_high_score = -1;
static int prev_draw_high_score_next = -1;


// read list of high scores
bool high_score_read()
{
	FILE* fp = fopen(FILE_NAME, "r");
	if (fp == NULL)
	{
		// not exist - create empty
		memset(&high_scores, 0, NUMBER_OF_HIGH_SCORES * sizeof(HighScore));
		return true;
	}

	// read all from file
	int count = 0;
	while (!feof(fp) && count < NUMBER_OF_HIGH_SCORES)
	{
		int result = fscanf(fp, "%d\t%d\t%d\t%d\n",
				&(high_scores[count].score), &(high_scores[count].time), &(high_scores[count].level), &(high_scores[count].lines));

		if (result != EOF)
		{
			++count;
		}
	}

	fclose(fp);
	fp = NULL;

	return true;
}

// save list of high scores
bool high_score_save()
{
	FILE* fp = fopen(FILE_NAME, "w");
	if (fp == NULL)
	{
		return false;
	}

	// write all to file
	for (int i = 0; i< NUMBER_OF_HIGH_SCORES; ++i)
	{
		fprintf(fp, "%d\t%d\t%d\t%d\n",
				high_scores[i].score, high_scores[i].time, high_scores[i].level, high_scores[i].lines);
	}

	fclose(fp);
	fp = NULL;

	return true;
}

// compare two high_scores, returns true if a >= b
bool high_score_better(HighScore* a, HighScore* b)
{
	if (a->score > b->score)
	{
		return true;
	}
	else if (a->score < b->score)
	{
		return false;
	}

	// same score - check time
	if (a->time < b->time)
	{
		return true;
	}
	else if (a->time > b->time)
	{
		return false;
	}

	// same time - check level
	if (a->level > b->level)
	{
		return true;
	}
	else if (a->level < b->level)
	{
		return false;
	}

	// same level - check lines
	if (a->lines > b->lines)
	{
		return true;
	}
	else if (a->lines < b->lines)
	{
		return false;
	}

	// equal - let a win
	return true;
}

int high_score_find_position(HighScore* hs)
{
	int position = -1;

	// check against all in list
	for (int i = 0; i < NUMBER_OF_HIGH_SCORES && position < 0; ++i)
	{
		if (high_score_better(hs, &(high_scores[i])))
		{
			position = i;
		}
	}

	return position;
}

void high_score_draw(int score, int time, int level, int lines)
{
	// load highscores
	if (dirty)
	{
		if (!high_score_read())
		{
			return;
		}
		dirty = false;
	}

	if (prev_draw_high_score != high_scores[0].score)
	{
		ui_draw_high_score(high_scores[0].score);
		prev_draw_high_score = high_scores[0].score;
	}
	if (score > prev_draw_high_score_next && prev_draw_high_score_next != 0)
	{
		// we have passed the the next high score on list

		HighScore hs;
		hs.score = score;
		hs.time = time;
		hs.level = level;
		hs.lines = lines;
		int position = high_score_find_position(&hs);
		if (position == 0)
		{
			// we have beaten the high score, no new goal
			ui_draw_high_score_next(high_scores[0].score, 1);
			prev_draw_high_score_next = 0;
		}
		else
		{
			if (position < 0)
			{
				// we are not on the list
				position = NUMBER_OF_HIGH_SCORES;
			}

			// show the next one on the list
			ui_draw_high_score_next(high_scores[position - 1].score, position);
			prev_draw_high_score_next = high_scores[position - 1].score;
		}
	}
}

int high_score_check(int score, int time, int level, int lines)
{
	// read from file
	if (dirty)
	{
		if (!high_score_read())
		{
			return -2;
		}
		dirty = false;
	}

	// create struct with new score
	HighScore s;
	s.score = score;
	s.time = time;
	s.level = level;
	s.lines = lines;

	int position = high_score_find_position(&s);

	if (position < 0)
	{
		// not on list
		return -1;
	}

	// insert score at position
	if (NUMBER_OF_HIGH_SCORES > 1)
	{
		for (int i = NUMBER_OF_HIGH_SCORES - 2; i >= position; --i)
		{
			high_scores[i+1] = high_scores[i];
		}
	}
	high_scores[position] = s;

	// save to file
	if (!high_score_save())
	{
		return -3;
	}

	dirty = true;

	return position + 1;
}

void high_score_new_game()
{
	prev_draw_high_score = -1;
	prev_draw_high_score_next = -1;
}
