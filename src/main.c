#include <stdio.h>
#include <unistd.h>
#include "ui.h"
#include "game.h"
#include "board.h"


int main(int argc, char* argv[])
{
	printf("Start\n");

	if (!ui_init())
	{
		return 1;
	}

	board_init();
	game_init();

	game_start();
	while (game_update())
	{
	}

	ui_done();

	printf("Done\n");

	return 0;
}

