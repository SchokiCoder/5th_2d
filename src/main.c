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

static const int FONT_SIZE = 16;

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

typedef struct MenuData
{
	SGUI_Menu *mnu_main;
	SGUI_Menu *mnu_start_game;
	SGUI_Menu *mnu_settings;
} MenuData ;

typedef struct BtnSettingsData
{
	MenuData *menu_data;
	Config *cfg;
    SGUI_Entry *txt_gfx_window_w;
    SGUI_Entry *txt_gfx_window_h;
    SGUI_Entry *txt_gfx_window_fullscreen;
} BtnSettingsData ;

typedef struct GameData
{
	char *world_name;
	SDL_Renderer *renderer;
	Config *cfg;
} GameData ;

void btn_start_game_click( void *ptr )
{
	MenuData *data = (MenuData*) ptr;

	data->mnu_main->active = false;
	data->mnu_start_game->visible = true;
}

void btn_settings_click( void *ptr )
{
	BtnSettingsData *data = (BtnSettingsData*) ptr;

	// toggle menus
	data->menu_data->mnu_main->active = false;
	data->menu_data->mnu_settings->visible = true;

	// update setting entries
	sprintf(data->txt_gfx_window_w->text.str, "%i", data->cfg->gfx_window_w);
	data->txt_gfx_window_w->text.len = SM_strlen(data->txt_gfx_window_w->text.str);
	SGUI_Entry_update_sprites(data->txt_gfx_window_w);

	sprintf(data->txt_gfx_window_h->text.str, "%i", data->cfg->gfx_window_h);
	data->txt_gfx_window_h->text.len = SM_strlen(data->txt_gfx_window_h->text.str);
	SGUI_Entry_update_sprites(data->txt_gfx_window_h);

	sprintf(data->txt_gfx_window_fullscreen->text.str, "%i", data->cfg->gfx_window_fullscreen);
	data->txt_gfx_window_fullscreen->text.len = SM_strlen(data->txt_gfx_window_fullscreen->text.str);
	SGUI_Entry_update_sprites(data->txt_gfx_window_fullscreen);
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
	MenuData *data = (MenuData*) ptr;

	data->mnu_main->active = true;
	data->mnu_start_game->visible = false;
}

void btn_settings_close_click( void *ptr )
{
	BtnSettingsData *data = (BtnSettingsData*) ptr;

	// toggle menus
	data->menu_data->mnu_main->active = true;
	data->menu_data->mnu_settings->visible = false;

	// save changes to cfg
    data->cfg->gfx_window_w = strtol(data->txt_gfx_window_w->text.str, NULL, 10);
    data->cfg->gfx_window_h = strtol(data->txt_gfx_window_h->text.str, NULL, 10);
    data->cfg->gfx_window_fullscreen = strtol(data->txt_gfx_window_fullscreen->text.str, NULL, 10);
}

void btn_chapter1_click( void *ptr )
{
	GameData *data = (GameData*) ptr;

	game_run(data->world_name, data->renderer, data->cfg);
}

/*
void gen_demo_horizontal( void )
{
	#include "world.h"
	#include "entity.h"

	World out = World_new("test", 128, 128);

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

	out.entities[0].id = E_PLAYER;
	out.entities[0].rect.x = 2.0f * BLOCK_SIZE;
	out.entities[0].rect.y = 1.0f * BLOCK_SIZE;
	out.entities[0].rect.w = DATA_ENTITIES[E_PLAYER].width;
	out.entities[0].rect.h = DATA_ENTITIES[E_PLAYER].height;

	World_write(&out);
	World_clear(&out);
}*/

int main()
{
	SM_String window_title = SM_String_new(64);
	SM_String msg = SM_String_new(16);
	SDL_Window *window;
	SDL_Renderer *renderer = NULL;
	TTF_Font *font;
	bool main_active = true;
	SDL_Event event;
	Config cfg = Config_new();

	SGUI_Menu mnu_main;
	SGUI_Label lbl_main;
	SGUI_Button btn_start_game;
	SGUI_Button btn_settings;
	SGUI_Button btn_exit;
	SGUI_Button btn_version;

	SGUI_Menu mnu_start_game;
	SGUI_Button btn_start_game_close;
	SGUI_Label lbl_start_game;
	SGUI_Button btn_chapter1;

	SGUI_Menu mnu_settings;
	SGUI_Button btn_settings_close;
	SGUI_Label lbl_gfx_window_w;
	SGUI_Entry txt_gfx_window_w;
	SGUI_Label lbl_gfx_window_h;
	SGUI_Entry txt_gfx_window_h;
	SGUI_Label lbl_gfx_window_fullscreen;
	SGUI_Entry txt_gfx_window_fullscreen;

	MenuData menu_data = {
		.mnu_main = &mnu_main,
		.mnu_start_game = &mnu_start_game,
		.mnu_settings = &mnu_settings,
	};

	BtnSettingsData btn_settings_data = {
		.cfg = &cfg,
		.menu_data = &menu_data,
		.txt_gfx_window_w = &txt_gfx_window_w,
		.txt_gfx_window_h = &txt_gfx_window_h,
		.txt_gfx_window_fullscreen = &txt_gfx_window_fullscreen,
	};

	// open log file and check
	SM_logfile = fopen(SM_PATH_LOG, "a");

	if (SM_logfile == NULL)
	{
		printf("Log file \"%s\" could not be opened.\nAbort.\n", SM_PATH_LOG);
		goto main_clear;
	}

	// load config
    Config_load(&cfg);

    // parse config values
    int window_mode = SDL_WINDOW_SHOWN;

    if (cfg.gfx_window_fullscreen)
		window_mode = SDL_WINDOW_FULLSCREEN;

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

    // load font
    font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf", FONT_SIZE);

    // create window title
    SM_String_copy_cstr(&window_title, APP_NAME);
    SM_String_append_cstr(&window_title, ": ");
    SM_String_append_cstr(&window_title, MOTDS[rand() % (sizeof(MOTDS) / sizeof(MOTDS[0]))]);

    // create window and renderer
    window = SDL_CreateWindow(
    	window_title.str,
    	cfg.gfx_window_x, cfg.gfx_window_y,
    	cfg.gfx_window_w, cfg.gfx_window_h,
    	window_mode);
    renderer = SDL_CreateRenderer(window, -1, 0);

    // check window and renderer
    if (window == NULL || renderer == NULL)
    {
    	SM_log_err("SDL could not open a window and renderer.");
    	goto main_clear;
    }

    // game data
    GameData game_data = {
		.world_name = "test",
		.renderer = renderer,
		.cfg = &cfg,
	};

    // enable alpha blending
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // make menus
    mnu_main = SGUI_Menu_new(renderer, THEME_MAIN.menu);
    SGUI_Label_new(&lbl_main, &mnu_main, font, THEME_MAIN.label);
    SGUI_Button_new(&btn_start_game, &mnu_main, font, THEME_MAIN.button);
    SGUI_Button_new(&btn_settings, &mnu_main, font, THEME_MAIN.button);
    SGUI_Button_new(&btn_exit, &mnu_main, font, THEME_MAIN.button);
    SGUI_Button_new(&btn_version, &mnu_main, font, THEME_MAIN.button);

    mnu_start_game = SGUI_Menu_new(renderer, THEME_WINDOW.menu);
    SGUI_Button_new(&btn_start_game_close, &mnu_start_game, font, THEME_WINDOW.button);
    SGUI_Label_new(&lbl_start_game, &mnu_start_game, font, THEME_WINDOW.label);
    SGUI_Button_new(&btn_chapter1, &mnu_start_game, font, THEME_WINDOW.button);

    mnu_settings = SGUI_Menu_new(renderer, THEME_WINDOW.menu);
    SGUI_Button_new(&btn_settings_close, &mnu_settings, font, THEME_WINDOW.button);
    SGUI_Label_new(&lbl_gfx_window_w, &mnu_settings, font, THEME_WINDOW.label);
    SGUI_Entry_new(&txt_gfx_window_w, &mnu_settings, font, THEME_WINDOW.entry);
    SGUI_Label_new(&lbl_gfx_window_h, &mnu_settings, font, THEME_WINDOW.label);
    SGUI_Entry_new(&txt_gfx_window_h, &mnu_settings, font, THEME_WINDOW.entry);
    SGUI_Label_new(&lbl_gfx_window_fullscreen, &mnu_settings, font, THEME_WINDOW.label);
    SGUI_Entry_new(&txt_gfx_window_fullscreen, &mnu_settings, font, THEME_WINDOW.entry);

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
    btn_start_game.data_click = &menu_data;

    SM_String_copy_cstr(&btn_settings.text, "Settings");
    SGUI_Button_update_sprite(&btn_settings);
    btn_settings.rect.w = btn_settings.sprite.surface->w;
    btn_settings.rect.h = btn_settings.sprite.surface->h;
    btn_settings.rect.x = btn_start_game.rect.x;
    btn_settings.rect.y = btn_start_game.rect.y + (btn_start_game.rect.h * 2);
    btn_settings.func_click = btn_settings_click;
    btn_settings.data_click = &btn_settings_data;

    SM_String_copy_cstr(&btn_exit.text, "Exit");
    SGUI_Button_update_sprite(&btn_exit);
    btn_exit.rect.w = btn_exit.sprite.surface->w;
    btn_exit.rect.h = btn_exit.sprite.surface->h;
    btn_exit.rect.x = btn_settings.rect.x;
    btn_exit.rect.y = btn_settings.rect.y + (btn_settings.rect.h * 2);
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
    btn_start_game_close.data_click = &menu_data;

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

    mnu_settings.rect.x = 200;
    mnu_settings.rect.y = 200;
    mnu_settings.rect.w = 400;
    mnu_settings.rect.h = 400;
    mnu_settings.visible = false;

    SM_String_copy_cstr(&btn_settings_close.text, "x");
    SGUI_Button_update_sprite(&btn_settings_close);
    btn_settings_close.rect.w = btn_settings_close.sprite.surface->w;
    btn_settings_close.rect.h = btn_settings_close.sprite.surface->h;
    btn_settings_close.rect.x =
    	mnu_settings.rect.x + mnu_settings.rect.w - btn_settings_close.rect.w;
    btn_settings_close.rect.y = mnu_settings.rect.y;
    btn_settings_close.func_click = btn_settings_close_click;
    btn_settings_close.data_click = &btn_settings_data;

    SM_String_copy_cstr(&lbl_gfx_window_w.text, "Resolution X");
    SGUI_Label_update_sprite(&lbl_gfx_window_w);
    lbl_gfx_window_w.rect.w = lbl_gfx_window_w.sprite.surface->w;
    lbl_gfx_window_w.rect.h = lbl_gfx_window_w.sprite.surface->h;
    lbl_gfx_window_w.rect.x = mnu_settings.rect.x + 25;
    lbl_gfx_window_w.rect.y = mnu_settings.rect.y;

    txt_gfx_window_w.rect.w = 200;
    txt_gfx_window_w.rect.h = FONT_SIZE;
    txt_gfx_window_w.rect.x = lbl_gfx_window_w.rect.x + lbl_gfx_window_w.rect.w + 25;
    txt_gfx_window_w.rect.y = lbl_gfx_window_w.rect.y;

    SM_String_copy_cstr(&lbl_gfx_window_h.text, "Resolution Y");
    SGUI_Label_update_sprite(&lbl_gfx_window_h);
    lbl_gfx_window_h.rect.w = lbl_gfx_window_h.sprite.surface->w;
    lbl_gfx_window_h.rect.h = lbl_gfx_window_h.sprite.surface->h;
    lbl_gfx_window_h.rect.x = lbl_gfx_window_w.rect.x;
    lbl_gfx_window_h.rect.y = lbl_gfx_window_w.rect.y + (lbl_gfx_window_w.rect.h * 2);

    txt_gfx_window_h.rect.w = 200;
    txt_gfx_window_h.rect.h = FONT_SIZE;
    txt_gfx_window_h.rect.x = lbl_gfx_window_h.rect.x + lbl_gfx_window_h.rect.w + 25;
    txt_gfx_window_h.rect.y = lbl_gfx_window_h.rect.y;

    SM_String_copy_cstr(&lbl_gfx_window_fullscreen.text, "Fullscreen");
    SGUI_Label_update_sprite(&lbl_gfx_window_fullscreen);
    lbl_gfx_window_fullscreen.rect.w = lbl_gfx_window_h.sprite.surface->w;
    lbl_gfx_window_fullscreen.rect.h = lbl_gfx_window_h.sprite.surface->h;
    lbl_gfx_window_fullscreen.rect.x = lbl_gfx_window_h.rect.x;
    lbl_gfx_window_fullscreen.rect.y = lbl_gfx_window_h.rect.y + (lbl_gfx_window_w.rect.h * 2);

    txt_gfx_window_fullscreen.rect.w = 20;
    txt_gfx_window_fullscreen.rect.h = FONT_SIZE;
    txt_gfx_window_fullscreen.rect.x = lbl_gfx_window_fullscreen.rect.x + lbl_gfx_window_fullscreen.rect.w + 25;
    txt_gfx_window_fullscreen.rect.y = lbl_gfx_window_fullscreen.rect.y;

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
			SGUI_Menu_handle_event(&mnu_settings, &event);

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
			SGUI_Menu_draw(&mnu_settings);

			SDL_RenderPresent(renderer);

			ts_draw = now();
		}
    }

	// save config
	Config_save(&cfg);

	main_clear:

	// clear strings
	SM_String_clear(&window_title);
	SM_String_clear(&msg);

	// clear menus
	SGUI_Menu_clear(&mnu_main);
	SGUI_Menu_clear(&mnu_start_game);
	SGUI_Menu_clear(&mnu_settings);

	// quit TTF
	TTF_Quit();

    // quit SDL
    SDL_Quit();

    //close log file
    fclose(SM_logfile);

	return 0;
}
