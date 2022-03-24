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
#include "app.h"
#include "motd.h"

static const SGUI_Theme THEME_CHEMARIUM = {
	.menu_bg_color = {.r = 155, .g = 219, .b = 245, .a = 255},

	.label_font_color = {.r = 50, .g = 50, .b = 50, .a = 255},
    .label_bg_color = {.r = 0, .g = 0, .b = 0, .a = 0},
    .label_border_color = {.r = 0, .g = 0, .b = 0, .a = 0},

    .button_font_color = {.r = 0, .g = 0, .b = 0, .a = 255},
    .button_bg_color = {.r = 255, .g = 255, .b = 255, .a = 255},
    .button_border_color = {.r = 0, .g = 0, .b = 0, .a = 255},
    .button_disabled_color = {.r = 0, .g = 0, .b = 0, .a = 50},

    .entry_font_color = {.r = 0, .g = 0, .b = 0, .a = 255},
    .entry_bg_color = {.r = 240, .g = 240, .b = 240, .a = 255},
    .entry_border_color = {.r = 0, .g = 0, .b = 0, .a = 255},
    .entry_disabled_color = {.r = 0, .g = 0, .b = 0, .a = 50}
};

int main()
{
	SM_String window_title = SM_String_new(64);
	SM_String appendage;
	SM_String msg = SM_String_new(16);
	SDL_Window *window;
	SDL_Renderer *renderer;
	TTF_Font *font;
	bool active = true;
	SDL_Event event;

	SGUI_Menu mnu_main;
	SGUI_Label lbl_main;
	SGUI_Button btn_start_game;

	SGUI_Menu mnu_start_game;
	SGUI_Button btn_start_game_close;
	SGUI_Label lbl_start_game;

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
    }

    // load font
    font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf", 16);

    // create window title
    appendage = SM_String_contain(APP_NAME);
    SM_String_copy(&window_title, &appendage);

    appendage = SM_String_contain(": ");
    SM_String_append(&window_title, &appendage);

    appendage = SM_String_contain(MOTDS[rand() % sizeof(MOTDS) / sizeof(MOTDS[0])]);
    SM_String_append(&window_title, &appendage);

    // create window and renderer
    window = SDL_CreateWindow(
    	window_title.str,
    	SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    	640, 480,
    	SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, 0);

    // check window and renderer
    if (window == NULL || renderer == NULL)
    {
    	SM_log_err("SDL could not open a window and renderer.");
    	goto main_clear;
    }

    // enable alpha blending
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // make menus
    mnu_main = SGUI_Menu_new(renderer, font, &THEME_CHEMARIUM);
    SGUI_Label_new(&lbl_main, &mnu_main, &THEME_CHEMARIUM);
    SGUI_Button_new(&btn_start_game, &mnu_main, &THEME_CHEMARIUM);

    mnu_start_game = SGUI_Menu_new(renderer, font, &THEME_CHEMARIUM);
    SGUI_Button_new(&btn_start_game_close, &mnu_start_game, &THEME_CHEMARIUM);
    SGUI_Label_new(&lbl_start_game, &mnu_start_game, &THEME_CHEMARIUM);

    // define menus
    mnu_main.rect.x = 0;
    mnu_main.rect.y = 0;
    mnu_main.rect.w = 640;
    mnu_main.rect.h = 480;

    appendage = SM_String_contain(APP_NAME);
    SM_String_copy(&lbl_main.text, &appendage);
    SGUI_Label_update_sprite(&lbl_main);
    lbl_main.rect.w = lbl_main.sprite.surface->w;
    lbl_main.rect.h = lbl_main.sprite.surface->h;
    lbl_main.rect.x = 50;
    lbl_main.rect.y = 200;

    appendage = SM_String_contain("Start game");
    SM_String_copy(&btn_start_game.text, &appendage);
    SGUI_Button_update_sprite(&btn_start_game);
    btn_start_game.rect.w = btn_start_game.sprite.surface->w;
    btn_start_game.rect.h = btn_start_game.sprite.surface->h;
    btn_start_game.rect.x = 75;
    btn_start_game.rect.y = lbl_main.rect.y + (lbl_main.rect.h * 2);

    mnu_start_game.rect.x = 100;
    mnu_start_game.rect.y = 100;
    mnu_start_game.rect.w = 400;
    mnu_start_game.rect.h = 200;
    mnu_start_game.visible = false;

    while (active)
    {
		// process events
		while (SDL_PollEvent(&event))
		{
			// menu events
			SGUI_Menu_handle_events(&mnu_main, &event);
			SGUI_Menu_handle_events(&mnu_start_game, &event);

			// app events
			switch (event.type)
			{
			case SDL_QUIT:
				active = false;
                break;
			}
		}

		// draw menus
		SGUI_Menu_draw(&mnu_main);
		SGUI_Menu_draw(&mnu_start_game);

		SDL_RenderPresent(renderer);
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
