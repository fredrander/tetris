#ifndef __highscore_h__
#define __highscore_h__


// draw high scores
void high_score_draw(int score, int time, int level, int lines);

// check if highscore, add to list if it is
// returns highscore rank or -1 if not on list
int high_score_check(int score, int time, int level, int lines);

// tell high score manager that we start new game
void high_score_new_game();


#endif // __highscore_h__
