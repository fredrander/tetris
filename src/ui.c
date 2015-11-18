#include "ui.h"
#include "uidefs.h"
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>


// background surface
static SDL_Surface* background;

// fonst
static TTF_Font* score_font = NULL;
static TTF_Font* label_font = NULL;

// Tetriminos sprites
static SDL_Surface* tetriminos_sprite[7];

// squares in all tetriminos and all directions
static SDL_Rect tetriminos_square[7][4][4];

static const char tetriminos_square_pos[7][4][4][2]=
{
	// I
	{
		{ {0,0}, {1,0}, {2,0}, {3,0} }, // rotation 0
		{ {1,-1}, {1,0}, {1,1}, {1,2} }, // rotation 90
		{ {0,0}, {1,0}, {2,0}, {3,0} }, // rotation 180
		{ {1,-1}, {1,0}, {1,1}, {1,2} }, // rotation 270
	},
	// J
	{
		{ {0,0}, {0,1}, {1,1}, {2,1} }, // rotation 0
		{ {2,0}, {2,1}, {2,2}, {1,2} }, // rotation 90
		{ {0,0}, {1,0}, {2,0}, {2,1} }, // rotation 180
		{ {1,0}, {1,1}, {1,2}, {2,0} }  // rotation 270
	},
	// L
	{
		{ {0,1}, {1,1}, {2,1}, {2,0} }, // rotation 0
		{ {1,0}, {2,0}, {2,1}, {2,2} }, // rotation 90
		{ {0,0}, {1,0}, {2,0}, {0,1} }, // rotation 180
		{ {1,0}, {1,1}, {1,2}, {2,2} }  // rotation 270
	},
	// O
	{
		{ {0,0}, {1,0}, {0,1}, {1,1} }, // rotation 0
		{ {0,0}, {1,0}, {0,1}, {1,1} }, // rotation 90
		{ {0,0}, {1,0}, {0,1}, {1,1} }, // rotation 180
		{ {0,0}, {1,0}, {0,1}, {1,1} }, // rotation 270
	},
	// S
	{
		{ {0,1}, {1,0}, {1,1}, {2,0} }, // rotation 0
		{ {1,0}, {1,1}, {2,1}, {2,2} }, // rotation 90
		{ {0,1}, {1,0}, {1,1}, {2,0} }, // rotation 180
		{ {1,0}, {1,1}, {2,1}, {2,2} }, // rotation 270
	},
	// T
	{
		{ {0,1}, {1,1}, {2,1}, {1,0} }, // rotation 0
		{ {2,0}, {2,1}, {1,1}, {2,2} }, // rotation 90
		{ {0,0}, {1,0}, {2,0}, {1,1} }, // rotation 180
		{ {1,0}, {1,1}, {1,2}, {2,1} }  // rotation 270
	},
	// Z
	{
		{ {0,0}, {1,0}, {1,1}, {2,1} }, // rotation 0
		{ {2,0}, {1,1}, {2,1}, {1,2} }, // rotation 90
		{ {0,0}, {1,0}, {1,1}, {2,1} }, // rotation 180
		{ {2,0}, {1,1}, {2,1}, {1,2} }, // rotation 270
	}
};

bool ui_setup_graphics()
{
	if (SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0)
	{
		printf("Failed to initialize SDL (%s)\n", SDL_GetError());
		return false;
	}

	background = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 0, SDL_SWSURFACE);
	if ( background == NULL )
	{
		printf("Unable to set %dx%d video: %s\n", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_GetError());
		return false;
	}

	// hide mouse
	SDL_ShowCursor(SDL_DISABLE);

	return true;
}

void ui_draw_label(const char* txt, unsigned short x, unsigned short y, unsigned short w, unsigned short h)
{
	SDL_Color color;
	color.r = SCORE_COLOR_R;
	color.g = SCORE_COLOR_G;
	color.b = SCORE_COLOR_B;

	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;

	SDL_Surface* surface = TTF_RenderText_Blended(label_font, txt, color);
	if (surface != NULL)
	{
		SDL_FillRect(background, &rect, SDL_MapRGB(background->format, BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B));
		SDL_BlitSurface(surface, NULL, background, &rect);
	    SDL_UpdateRect(background, x, y, w, h);
		SDL_FreeSurface(surface);
	}
}

void ui_draw_labels()
{
	ui_draw_label("Score:", SCORE_LABEL_AREA_X, SCORE_LABEL_AREA_Y, SCORE_LABEL_AREA_WIDTH, SCORE_LABEL_AREA_HEIGHT);
	ui_draw_label("Level:", LEVEL_LABEL_AREA_X, LEVEL_LABEL_AREA_Y, LEVEL_LABEL_AREA_WIDTH, LEVEL_LABEL_AREA_HEIGHT);
	ui_draw_label("Lines:", LINES_LABEL_AREA_X, LINES_LABEL_AREA_Y, LINES_LABEL_AREA_WIDTH, LINES_LABEL_AREA_HEIGHT);
	ui_draw_label("Time:", TIME_LABEL_AREA_X, TIME_LABEL_AREA_Y, TIME_LABEL_AREA_WIDTH, TIME_LABEL_AREA_HEIGHT);
	ui_draw_label("High score:", HIGH_SCORE_LABEL_AREA_X, HIGH_SCORE_LABEL_AREA_Y, HIGH_SCORE_LABEL_AREA_WIDTH, HIGH_SCORE_LABEL_AREA_HEIGHT);
	ui_draw_label("Position #10:", HIGH_SCORE_NEXT_LABEL_AREA_X, HIGH_SCORE_NEXT_LABEL_AREA_Y, HIGH_SCORE_NEXT_LABEL_AREA_WIDTH, HIGH_SCORE_NEXT_LABEL_AREA_HEIGHT);
	ui_draw_label("Next:", NEXT_LABEL_X, NEXT_LABEL_Y, NEXT_LABEL_WIDTH, NEXT_LABEL_HEIGHT);
}

void ui_create_background()
{
	SDL_Rect background_rect_v[4];

	background_rect_v[0].x = 0;
	background_rect_v[0].y = 0;
	background_rect_v[0].w = SCREEN_WIDTH;
	background_rect_v[0].h = GAME_AREA_Y;
	
	background_rect_v[1].x = GAME_AREA_X + GAME_AREA_WIDTH;
	background_rect_v[1].y = GAME_AREA_Y;
	background_rect_v[1].w = SCREEN_WIDTH - GAME_AREA_X - GAME_AREA_WIDTH;
	background_rect_v[1].h = GAME_AREA_HEIGHT;

	background_rect_v[2].x = 0;
	background_rect_v[2].y = GAME_AREA_Y + GAME_AREA_HEIGHT;
	background_rect_v[2].w = SCREEN_WIDTH;
	background_rect_v[2].h = SCREEN_HEIGHT - GAME_AREA_Y - GAME_AREA_HEIGHT;

	background_rect_v[3].x = 0;
	background_rect_v[3].y = GAME_AREA_Y;
	background_rect_v[3].w = GAME_AREA_X;
	background_rect_v[3].h = GAME_AREA_HEIGHT;

	SDL_Rect game_rect;
	game_rect.x = GAME_AREA_X;
	game_rect.y = GAME_AREA_Y;
	game_rect.w = GAME_AREA_WIDTH;
	game_rect.h = GAME_AREA_HEIGHT;

	SDL_LockSurface(background);
	
	for (int i= 0; i< 4; ++i)
	{
		SDL_FillRect(background, &(background_rect_v[i]), SDL_MapRGB(background->format, BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B));
	}

	SDL_FillRect(background, &game_rect, SDL_MapRGB(background->format, GAME_AREA_COLOR_R, GAME_AREA_COLOR_G, GAME_AREA_COLOR_B));
	
	SDL_Flip(background);

	SDL_UnlockSurface(background);

	ui_draw_labels();
}


void ui_setup_tetriminos_square()
{
	// create rects from all square positions
	for (int tetrimino= 0; tetrimino < 7; ++tetrimino)
	{
		for (int rotation= 0; rotation < 4; ++rotation)
		{
			for (int square= 0; square < 4; ++square)
			{
				tetriminos_square[tetrimino][rotation][square].x = tetriminos_square_pos[tetrimino][rotation][square][0];
				tetriminos_square[tetrimino][rotation][square].y = tetriminos_square_pos[tetrimino][rotation][square][1];
				tetriminos_square[tetrimino][rotation][square].w = 1;
				tetriminos_square[tetrimino][rotation][square].h = 1;
			}
		}
	} 
}

void ui_load_tetriminos()
{
	SDL_Surface* temp;
	int colorkey;
	char* bmp_files[7];

	bmp_files[0] = "../res/i.bmp";
	bmp_files[1] = "../res/j.bmp";
	bmp_files[2] = "../res/l.bmp";
	bmp_files[3] = "../res/o.bmp";
	bmp_files[4] = "../res/s.bmp";
	bmp_files[5] = "../res/t.bmp";
	bmp_files[6] = "../res/z.bmp";

	// white as sprite colorkey
	colorkey = SDL_MapRGB(background->format, 255, 255, 255);

	// load all figures
	for (int i= 0; i< 7; ++i)
	{
		// load bmp
		temp = SDL_LoadBMP(bmp_files[i]);
		tetriminos_sprite[i] = SDL_DisplayFormat(temp);
		SDL_FreeSurface(temp);

		SDL_SetColorKey(tetriminos_sprite[i], SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);
	}
	
	// setup squares for all Tetriminos
	ui_setup_tetriminos_square();
}

bool ui_init_font()
{
	// init font library
	if (TTF_Init() < 0)
	{
		return false;
	}

	score_font = TTF_OpenFont("../res/FreeSansBold.ttf", 32);
	label_font = TTF_OpenFont("../res/FreeSansBold.ttf", 12);
	if (score_font == NULL || label_font == NULL)
	{
		TTF_Quit();
		return false;
	}

	return true;
}

bool ui_init()
{
	if ( !ui_init_font())
	{
		return false;
	}

	if ( !ui_setup_graphics() )
	{
		return false;
	}

	// draw background
	ui_create_background();

	// load/initialize Tetriminos
	ui_load_tetriminos();

	return true;
}

void ui_done_font()
{
	TTF_CloseFont(score_font);
	score_font = NULL;
	TTF_CloseFont(label_font);
	label_font = NULL;

	TTF_Quit();
}

void ui_done()
{
	ui_done_font();

	for (int i= 0; i< 7; ++i)
	{
	    SDL_FreeSurface(tetriminos_sprite[i]);
	    tetriminos_sprite[i] = NULL;
	}

    SDL_FreeSurface(background);
    background = NULL;
	SDL_Quit();
}

const SDL_Rect* ui_get_tetrimino(Tetriminos tetrimino, Rotation rotation)
{
	return tetriminos_square[tetrimino][rotation];
}

void ui_draw_tetrimino(Tetriminos tetrimino, Rotation rotation, int game_pos_x, int game_pos_y)
{
	// draw all squares in Tetrimino (they all have 4 squares)
	for (int i= 0; i< 4; ++i)
	{
		ui_draw_tetrimino_square(tetrimino, 
			game_pos_x + tetriminos_square[tetrimino][rotation][i].x, 
			game_pos_y + tetriminos_square[tetrimino][rotation][i].y);
	}
}

void ui_clear_tetrimino(Tetriminos tetrimino, Rotation rotation, int game_pos_x, int game_pos_y)
{
	// draw all squares in tetrimino (they all have 4 squares)
	for (int i= 0; i< 4; ++i)
	{
		ui_clear_tetrimino_square( game_pos_x + tetriminos_square[tetrimino][rotation][i].x,
				game_pos_y + tetriminos_square[tetrimino][rotation][i].y);
	}
}

void ui_draw_tetrimino_square_world_pos(Tetriminos tetrimino, SDL_Rect* pos)
{
	SDL_BlitSurface(tetriminos_sprite[tetrimino], NULL, background, pos);
}

void ui_draw_tetrimino_square(Tetriminos tetrimino, int game_pos_x, int game_pos_y)
{
	// create position
	SDL_Rect pos;

	pos.x = GAME_AREA_X + game_pos_x * GAME_AREA_SQUARE_SIZE;
	pos.y = GAME_AREA_Y + game_pos_y * GAME_AREA_SQUARE_SIZE;

	ui_draw_tetrimino_square_world_pos(tetrimino, &pos);
}

void ui_clear_tetrimino_square(int game_pos_x, int game_pos_y)
{
	// create position
	SDL_Rect clear_rect;

	clear_rect.x = GAME_AREA_X + game_pos_x * GAME_AREA_SQUARE_SIZE;
	clear_rect.y = GAME_AREA_Y + game_pos_y * GAME_AREA_SQUARE_SIZE;
	clear_rect.w = GAME_AREA_SQUARE_SIZE;
	clear_rect.h = GAME_AREA_SQUARE_SIZE;

	SDL_FillRect(background, &clear_rect, SDL_MapRGB(background->format, GAME_AREA_COLOR_R, GAME_AREA_COLOR_G, GAME_AREA_COLOR_B));
}

void ui_clear_game_area()
{
	SDL_Rect game_rect;
	game_rect.x = GAME_AREA_X;
	game_rect.y = GAME_AREA_Y;
	game_rect.w = GAME_AREA_WIDTH;
	game_rect.h = GAME_AREA_HEIGHT;

	SDL_FillRect(background, &game_rect, SDL_MapRGB(background->format, GAME_AREA_COLOR_R, GAME_AREA_COLOR_G, GAME_AREA_COLOR_B));
}

void ui_update_game_area()
{
	SDL_UpdateRect(background, GAME_AREA_X, GAME_AREA_Y, GAME_AREA_WIDTH, GAME_AREA_HEIGHT);
}

void ui_draw_score_value(int value, int x, int y, int w, int h)
{
	char txt[16];

	SDL_Color color;
	color.r = SCORE_COLOR_R;
	color.g = SCORE_COLOR_G;
	color.b = SCORE_COLOR_B;

	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;

	snprintf(txt, 16, "%.7d", value);

	SDL_Surface* surface = TTF_RenderText_Blended(score_font, txt, color);
	if (surface != NULL)
	{
		SDL_FillRect(background, &rect, SDL_MapRGB(background->format, BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B));
		SDL_BlitSurface(surface, NULL, background, &rect);
	    SDL_UpdateRect(background, x, y, w, h);
		SDL_FreeSurface(surface);
	}
}

void ui_draw_score(int score)
{
	ui_draw_score_value(score, SCORE_AREA_X, SCORE_AREA_Y, SCORE_AREA_WIDTH, SCORE_AREA_HEIGHT);
}

void ui_draw_level(int level)
{
	ui_draw_score_value(level, LEVEL_AREA_X, LEVEL_AREA_Y, LEVEL_AREA_WIDTH, LEVEL_AREA_HEIGHT);
}

void ui_draw_lines(int lines)
{
	ui_draw_score_value(lines, LINES_AREA_X, LINES_AREA_Y, LINES_AREA_WIDTH, LINES_AREA_HEIGHT);
}

void ui_draw_time(unsigned long time)
{
	ui_draw_score_value(time, TIME_AREA_X, TIME_AREA_Y, TIME_AREA_WIDTH, TIME_AREA_HEIGHT);
}

void ui_draw_high_score(int score)
{
	ui_draw_score_value(score, HIGH_SCORE_AREA_X, HIGH_SCORE_AREA_Y, HIGH_SCORE_AREA_WIDTH, HIGH_SCORE_AREA_HEIGHT);
}

void ui_draw_high_score_next(int score, int position)
{
	ui_draw_score_value(score, HIGH_SCORE_NEXT_AREA_X, HIGH_SCORE_NEXT_AREA_Y, HIGH_SCORE_NEXT_AREA_WIDTH, HIGH_SCORE_NEXT_AREA_HEIGHT);

	char txt[16];

	snprintf(txt, 16, "Position #%d:", position);
	ui_draw_label(txt, HIGH_SCORE_NEXT_LABEL_AREA_X, HIGH_SCORE_NEXT_LABEL_AREA_Y, HIGH_SCORE_NEXT_LABEL_AREA_WIDTH, HIGH_SCORE_NEXT_LABEL_AREA_HEIGHT);
}

void ui_draw_next(Tetriminos t)
{
	// clear area
	SDL_Rect rect;
	rect.x = NEXT_X;
	rect.y = NEXT_Y;
	rect.w = NEXT_WIDTH;
	rect.h = NEXT_HEIGHT;
	SDL_FillRect(background, &rect, SDL_MapRGB(background->format, BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B));

	// draw all squares in Tetrimino (they all have 4 squares)
	SDL_Rect pos;
	for (int i= 0; i< 4; ++i)
	{
		pos.x = NEXT_X + tetriminos_square[t][ROTATION_0][i].x * GAME_AREA_SQUARE_SIZE;
		pos.y = NEXT_Y + tetriminos_square[t][ROTATION_0][i].y * GAME_AREA_SQUARE_SIZE;

		ui_draw_tetrimino_square_world_pos(t, &pos);
	}
    SDL_UpdateRect(background, rect.x, rect.y, rect.w, rect.h);
}
