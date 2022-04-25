/*
	Chemarium
	Copyright (C) 2022	Andy Frank Schoknecht

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <SM_string.h>
#include <SM_log.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SGUI_theme.h>
#include <SGUI_menu.h>
#include <SGUI_label.h>
#include <SGUI_button.h>
#include <time.h>
#include "config.h"
#include "app.h"
#include "motd.h"
#include "game.h"

static const float MENU_FRAMERATE = 30.0f;

static const SGUI_Theme THEME_MAIN = {
	.menu = {
		.bg_color = {.r = 155, .g = 219, .b = 245, .a = 255},
	},

	.label = {
		.font_color = {.r = 50, .g = 50, .b = 200, .a = 255},
    	.bg_color = {.r = 0, .g = 0, .b = 0, .a = 0},
    	.border_color = {.r = 0, .g = 0, .b = 0, .a = 0},
	},

    .button = {
    	.font_color = {.r = 50, .g = 50, .b = 50, .a = 255},
    	.bg_color = {.r = 0, .g = 0, .b = 0, .a = 25},
    	.border_color = {.r = 0, .g = 0, .b = 0, .a = 0},
    	.disabled_color = {.r = 0, .g = 0, .b = 0, .a = 50},
	},

    .entry = {
    	.font_color = {.r = 0, .g = 0, .b = 0, .a = 255},
    	.bg_color = {.r = 240, .g = 240, .b = 240, .a = 255},
    	.border_color = {.r = 0, .g = 0, .b = 0, .a = 255},
    	.disabled_color = {.r = 0, .g = 0, .b = 0, .a = 50},
	},
};

static const SGUI_Theme THEME_WINDOW = {
	.menu = {
		.bg_color = {.r = 0, .g = 0, .b = 0, .a = 50},
	},

	.label = {
		.font_color = {.r = 50, .g = 50, .b = 200, .a = 255},
    	.bg_color = {.r = 0, .g = 0, .b = 0, .a = 0},
    	.border_color = {.r = 0, .g = 0, .b = 0, .a = 0},
	},

    .button = {
    	.font_color = {.r = 20, .g = 255, .b = 20, .a = 255},
    	.bg_color = {.r = 0, .g = 0, .b = 0, .a = 0},
    	.border_color = {.r = 255, .g = 255, .b = 255, .a = 20},
    	.disabled_color = {.r = 0, .g = 0, .b = 0, .a = 50},
	},

    .entry = {
    	.font_color = {.r = 0, .g = 0, .b = 0, .a = 255},
    	.bg_color = {.r = 240, .g = 240, .b = 240, .a = 255},
    	.border_color = {.r = 0, .g = 0, .b = 0, .a = 255},
    	.disabled_color = {.r = 0, .g = 0, .b = 0, .a = 50},
	},
};

struct MenuPointers
{
	SGUI_Menu *mnu_main;
	SGUI_Menu *mnu_start_game;
};

struct GameData
{
	char *path_world;
	SDL_Renderer *renderer;
};

void btn_start_game_click( void *ptr )
{
	struct MenuPointers *data = (struct MenuPointers*) ptr;

	data->mnu_main->active = false;
	data->mnu_start_game->visible = true;
}

void btn_exit_click( void *ptr )
{
	bool *active = (bool*) ptr;

	*active = false;
}

void btn_version_click( )
{
	printf(
		"%s %u.%u.%u is licensed under the %s.\n" \
		"%s" \
		"The source code of this program is available at\n" \
		"%s\n",
		APP_NAME, APP_MAJOR, APP_MINOR, APP_PATCH, APP_LICENSE,
		APP_LICENSE_NOTICE,
		APP_SOURCE);
}

void btn_start_game_close_click( void *ptr )
{
	struct MenuPointers *data = (struct MenuPointers*) ptr;

	data->mnu_main->active = true;
	data->mnu_start_game->visible = false;
}

void btn_chapter1_click( void *ptr )
{
	struct GameData *data = (struct GameData*) ptr;

	game_run(data->path_world, data->renderer);
}

#include "world.h"
void gen_demo_horizontal( void ) {
	World out = World_new("test", 16, 16);

	out.blocks[0][0] = B_STONE;
	out.blocks[0][1] = B_STONE;
	out.blocks[0][5] = B_DIRT;

	out.blocks[1][5] = B_DIRT;
	out.blocks[1][6] = B_STONE;

	out.blocks[2][5] = B_DIRT;
	out.blocks[2][6] = B_STONE;

	out.blocks[3][5] = B_DIRT;
	out.blocks[3][6] = B_STONE;

	out.blocks[4][5] = B_DIRT;
	out.blocks[4][6] = B_STONE;

	out.blocks[5][5] = B_DIRT;
	out.blocks[5][6] = B_STONE;

	out.blocks[6][6] = B_STONE;

	out.blocks[7][6] = B_STONE;

	out.blocks[10][0] = B_STONE;
	out.blocks[10][1] = B_STONE;

	out.walls[0][3] = B_DIRT;
	out.walls[0][4] = B_DIRT;

	out.walls[1][3] = B_DIRT;
	out.walls[1][4] = B_DIRT;

	out.walls[2][3] = B_DIRT;
	out.walls[2][4] = B_DIRT;

	out.walls[3][3] = B_DIRT;
	out.walls[3][4] = B_DIRT;

	out.walls[4][4] = B_DIRT;

	out.entities[0].type = E_PLAYER,
	out.entities[0].rect.x = 2.0f * BLOCK_SIZE;
	out.entities[0].rect.y = 1.0f * BLOCK_SIZE;
	out.entities[0].rect.w = PLAYER_WIDTH;
	out.entities[0].rect.h = PLAYER_HEIGHT;

	World_write(&out);
	World_clear(&out);
}

int main()
{
	SM_String window_title = SM_String_new(64);
	SM_String msg = SM_String_new(16);
	SDL_Window *window;
	SDL_Renderer *renderer;
	TTF_Font *font;
	bool main_active = true;
	SDL_Event event;
	Config cfg = Config_new();

	SGUI_Menu mnu_main;
	SGUI_Label lbl_main;
	SGUI_Button btn_start_game;
	SGUI_Button btn_exit;
	SGUI_Button btn_version;

	SGUI_Menu mnu_start_game;
	SGUI_Button btn_start_game_close;
	SGUI_Label lbl_start_game;
	SGUI_Button btn_chapter1;

	struct MenuPointers menu_pointers = {
		.mnu_main = &mnu_main,
		.mnu_start_game = &mnu_start_game
	};

	// open log file and check
	SM_logfile = fopen(SM_PATH_LOG, "a");

	if (SM_logfile == NULL)
	{
		printf("Log file \"%s\" could not be opened.\nAbort.\n", SM_PATH_LOG);
		goto main_clear;
	}

	// seed random generator
	srand(time(NULL));

    // init SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
    	SM_log_err("SDL could not initialize.");
    	goto main_clear;
    }

    // init TTF
    if (TTF_Init() != 0)
    {
    	SM_log_err("TTF could not initialize.");
    	goto main_clear;
    }

    // load config
    Config_load(&cfg);

    // load font
    font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf", 16);

    // create window title
    SM_String_copy_cstr(&window_title, APP_NAME);
    SM_String_append_cstr(&window_title, ": ");
    SM_String_append_cstr(&window_title, MOTDS[rand() % (sizeof(MOTDS) / sizeof(MOTDS[0]))]);

    // create window and renderer
    window = SDL_CreateWindow(
    	window_title.str,
    	cfg.gfx_window_x, cfg.gfx_window_y,
    	cfg.gfx_window_w, cfg.gfx_window_h,
    	SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, 0);

    // check window and renderer
    if (window == NULL || renderer == NULL)
    {
    	SM_log_err("SDL could not open a window and renderer.");
    	goto main_clear;
    }

    // prep some data
	struct GameData game_data = {
		.path_world = "test",
		.renderer = renderer
	};

    // enable alpha blending
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // make menus
    mnu_main = SGUI_Menu_new(renderer, THEME_MAIN.menu);
    SGUI_Label_new(&lbl_main, &mnu_main, font, THEME_MAIN.label);
    SGUI_Button_new(&btn_start_game, &mnu_main, font, THEME_MAIN.button);
    SGUI_Button_new(&btn_exit, &mnu_main, font, THEME_MAIN.button);
    SGUI_Button_new(&btn_version, &mnu_main, font, THEME_MAIN.button);

    mnu_start_game = SGUI_Menu_new(renderer, THEME_WINDOW.menu);
    SGUI_Button_new(&btn_start_game_close, &mnu_start_game, font, THEME_WINDOW.button);
    SGUI_Label_new(&lbl_start_game, &mnu_start_game, font, THEME_WINDOW.label);
    SGUI_Button_new(&btn_chapter1, &mnu_start_game, font, THEME_WINDOW.button);

    // define menus
    mnu_main.rect.x = 0;
    mnu_main.rect.y = 0;
    mnu_main.rect.w = cfg.gfx_window_w;
    mnu_main.rect.h = cfg.gfx_window_h;

    SM_String_copy_cstr(&lbl_main.text, APP_NAME);
    SGUI_Label_update_sprite(&lbl_main);
    lbl_main.rect.w = lbl_main.sprite.surface->w;
    lbl_main.rect.h = lbl_main.sprite.surface->h;
    lbl_main.rect.x = 50;
    lbl_main.rect.y = 200;

    SM_String_copy_cstr(&btn_start_game.text, "Start game");
    SGUI_Button_update_sprite(&btn_start_game);
    btn_start_game.rect.w = btn_start_game.sprite.surface->w;
    btn_start_game.rect.h = btn_start_game.sprite.surface->h;
    btn_start_game.rect.x = 75;
    btn_start_game.rect.y = lbl_main.rect.y + (lbl_main.rect.h * 2);
    btn_start_game.func_click = btn_start_game_click;
    btn_start_game.data_click = &menu_pointers;

    SM_String_copy_cstr(&btn_exit.text, "Exit");
    SGUI_Button_update_sprite(&btn_exit);
    btn_exit.rect.w = btn_exit.sprite.surface->w;
    btn_exit.rect.h = btn_exit.sprite.surface->h;
    btn_exit.rect.x = btn_start_game.rect.x;
    btn_exit.rect.y = btn_start_game.rect.y + (btn_start_game.rect.h * 2);
    btn_exit.func_click = btn_exit_click;
    btn_exit.data_click = &main_active;

    sprintf(btn_version.text.str, "%u.%u.%u", APP_MAJOR, APP_MINOR, APP_PATCH);
    btn_version.text.len = strlen(btn_version.text.str);
    SGUI_Button_update_sprite(&btn_version);
    btn_version.rect.w = btn_version.sprite.surface->w;
    btn_version.rect.h = btn_version.sprite.surface->h;
    btn_version.rect.x = cfg.gfx_window_w - btn_version.rect.w - 5;
    btn_version.rect.y = cfg.gfx_window_h - btn_version.rect.h - 5;
    btn_version.func_click = btn_version_click;

    mnu_start_game.rect.x = 100;
    mnu_start_game.rect.y = 100;
    mnu_start_game.rect.w = 400;
    mnu_start_game.rect.h = 200;
    mnu_start_game.visible = false;

    SM_String_copy_cstr(&btn_start_game_close.text, "x");
    SGUI_Button_update_sprite(&btn_start_game_close);
    btn_start_game_close.rect.w = btn_start_game_close.sprite.surface->w;
    btn_start_game_close.rect.h = btn_start_game_close.sprite.surface->h;
    btn_start_game_close.rect.x =
    	mnu_start_game.rect.x + mnu_start_game.rect.w - btn_start_game_close.rect.w;
    btn_start_game_close.rect.y = mnu_start_game.rect.y;
    btn_start_game_close.func_click = btn_start_game_close_click;
    btn_start_game_close.data_click = &menu_pointers;

    SM_String_copy_cstr(&lbl_start_game.text, "Select a chapter");
    SGUI_Label_update_sprite(&lbl_start_game);
    lbl_start_game.rect.w = lbl_start_game.sprite.surface->w;
    lbl_start_game.rect.h = lbl_start_game.sprite.surface->h;
    lbl_start_game.rect.x = mnu_start_game.rect.x;
    lbl_start_game.rect.y = mnu_start_game.rect.y;

    SM_String_copy_cstr(&btn_chapter1.text, "test");
    SGUI_Button_update_sprite(&btn_chapter1);
    btn_chapter1.rect.w = btn_chapter1.sprite.surface->w;
    btn_chapter1.rect.h = btn_chapter1.sprite.surface->h;
    btn_chapter1.rect.x = lbl_start_game.rect.x + 25;
    btn_chapter1.rect.y = lbl_start_game.rect.y + (lbl_start_game.rect.h * 2);
    btn_chapter1.func_click = btn_chapter1_click;
    btn_chapter1.data_click = &game_data;

    // mainloop
    float ts_draw, ts_now;

    while (main_active)
    {
		// process events
		while (SDL_PollEvent(&event))
		{
			// menu events
			SGUI_Menu_handle_event(&mnu_main, &event);
			SGUI_Menu_handle_event(&mnu_start_game, &event);

			// app events
			switch (event.type)
			{
			case SDL_QUIT:
				main_active = false;
                break;
			}
		}

		ts_now = now();

		// draw menus
		if (ts_now > ts_draw + (1.0f / MENU_FRAMERATE))
		{
			SGUI_Menu_draw(&mnu_main);
			SGUI_Menu_draw(&mnu_start_game);

			SDL_RenderPresent(renderer);

			ts_draw = now();
		}
    }

	main_clear:

	// clear strings
	SM_String_clear(&window_title);
	SM_String_clear(&msg);

	// clear menus
	SGUI_Menu_clear(&mnu_main);
	SGUI_Menu_clear(&mnu_start_game);

	// quit TTF
	TTF_Quit();

    // quit SDL
    SDL_Quit();

    //close log file
    fclose(SM_logfile);

	return 0;
}
