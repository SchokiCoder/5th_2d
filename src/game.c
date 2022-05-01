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

#include <SM_log.h>
#include <SM_string.h>
#include <SGUI_sprite.h>
#include <SGUI_theme.h>
#include <time.h>
#include "types.h"
#include "config.h"
#include "world.h"
#include "block.h"
#include "entity.h"
#include "game.h"

#ifdef _WIN32
# define SLASH "\\"
#else
# define SLASH "/"
#endif

#define PATH_TEXTURES_BLOCKS_DIR PATH_TEXTURES "blocks"
#define PATH_TEXTURES_ENTITIES_DIR PATH_TEXTURES "entities"

static const char *PATH_TEXTURES_BLOCKS[] = {
	PATH_TEXTURES_BLOCKS_DIR SLASH "dirt.png",
	PATH_TEXTURES_BLOCKS_DIR SLASH "stone.png"
};

static const char *PATH_TEXTURES_ENTITIES[] = {
	PATH_TEXTURES_ENTITIES_DIR SLASH "player.png"
};

static const float TIMESCALE = 1.0f;

#ifdef _DEBUG
static const SGUI_Theme THEME_DEBUG = {
	.menu = {
		.bg_color = {.r = 155, .g = 219, .b = 245, .a = 0},
	},

	.label = {
		.font_color = {.r = 255, .g = 0, .b = 255, .a = 255},
    	.bg_color = {.r = 0, .g = 0, .b = 0, .a = 20},
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
#endif

float now( void )
{
	return (float) clock() / (float) CLOCKS_PER_SEC;
}

void game_run(
	const char *world_name, SDL_Renderer *renderer, Config *cfg,
	const bool edit, const size_t width, const size_t height )
{
#ifdef _DEBUG
	TTF_Font *font;
    SGUI_Menu mnu_debugvals;
    SGUI_Label lbl_velocity_x;
    SGUI_Label lbl_velocity_x_val;
    SGUI_Label lbl_velocity_y;
    SGUI_Label lbl_velocity_y_val;
    SGUI_Label lbl_pos_x;
    SGUI_Label lbl_pos_x_val;
    SGUI_Label lbl_pos_y;
    SGUI_Label lbl_pos_y_val;
    SGUI_Label lbl_grounded;
    SGUI_Label lbl_grounded_val;
#endif

	SM_String msg = SM_String_new(16);
	SGUI_Sprite spr_blocks[B_LAST + 1];
	SGUI_Sprite spr_walls[B_LAST + 1];
	SGUI_Sprite spr_ents[E_LAST + 1];
	World world = {.invalid = false};
	SDL_Event event;
    IPoint wld_draw_pts[2];
    WldEntity *player = NULL;
	SDL_Rect viewport;
	SDL_Rect temp;
	const uint8_t *kbd;
	bool game_active = true;
	float ts1, ts2, delta = 0.0f;
	float x_step = 0.0f;
	float y_step = 0.0f;

	// set viewport
	viewport.x = 0;
	viewport.y = 0;
	viewport.w = cfg->gfx_window_w;
	viewport.h = cfg->gfx_window_h;

	// init sprites
	for (uint_fast32_t i = 0; i <= B_LAST; i++)
	{
		spr_blocks[i] = SGUI_Sprite_new();
		spr_walls[i] = SGUI_Sprite_new();
	}

	for (uint_fast32_t i = 0; i <= E_LAST; i++)
		spr_ents[i] = SGUI_Sprite_new();

	// open world and check
	world = World_from_file(world_name);

	if (world.invalid)
	{
		SM_String_copy_cstr(&msg, "World ");
		SM_String_append_cstr(&msg, world_name);
		SM_String_append_cstr(&msg, " is corrupt.");

		SM_log_err(msg.str);
		goto game_clear;
	}

	// set 1st player of world as player
	for (size_t i = 0; i < world.ent_count; i++)
		if (world.entities[i].id == E_PLAYER)
			player = &world.entities[i];

	if (player == NULL)
	{
		SM_String_copy_cstr(&msg, "World ");
		SM_String_append_cstr(&msg, world_name);
		SM_String_append_cstr(&msg, " does not contain a player entity.");

		SM_log_err(msg.str);
		goto game_clear;
	}

	// load block sprites
    for (uint_fast32_t i = 1; i <= B_LAST; i++)
	{
		spr_blocks[i] = SGUI_Sprite_from_file(renderer, PATH_TEXTURES_BLOCKS[i - 1]);

		if (spr_blocks[i].invalid)
		{
			SM_String_copy_cstr(&msg, "Sprite ");
			SM_String_append_cstr(&msg, PATH_TEXTURES_BLOCKS[i - 1]);
			SM_String_append_cstr(&msg, " could not be loaded.");

			SM_log_err(msg.str);
			goto game_clear;
		}
	}

	// create wall sprites
    for (uint_fast32_t i = 1; i <= B_LAST; i++)
    {
    	// copy block surface, modify, create texture
    	spr_walls[i].invalid = false;
    	spr_walls[i].surface = SDL_ConvertSurface(spr_blocks[i].surface, spr_blocks[i].surface->format, 0);
		SDL_SetSurfaceColorMod(spr_walls[i].surface, 175, 175, 175);
    	SGUI_Sprite_create_texture(&spr_walls[i], renderer);

    	// check sprite
    	if (spr_walls[i].invalid)
    	{
    		SM_log_err("Wall-sprite could not be generated from block-sprite.");
    		goto game_clear;
		}
    }

	// load ent sprites
	for (uint_fast32_t i = 1; i <= E_LAST; i++)
	{
		spr_ents[i] = SGUI_Sprite_from_file(renderer, PATH_TEXTURES_ENTITIES[i - 1]);

		if (spr_ents[i].invalid)
		{
			SM_String_copy_cstr(&msg, "Sprite ");
			SM_String_append_cstr(&msg, PATH_TEXTURES_ENTITIES[i - 1]);
			SM_String_append_cstr(&msg, " could not be loaded.");

			SM_log_err(msg.str);
			goto game_clear;
		}
	}

    // map textures
    for (uint_fast32_t x = 0; x < world.width; x++)
    {
    	for (uint_fast32_t y = 0; y < world.height; y++)
    	{
            world.block_textures[x][y] = spr_blocks[world.blocks[x][y]].texture;
            world.wall_textures[x][y] = spr_walls[world.walls[x][y]].texture;
		}
	}

	// set keyboard state pointer
	kbd = SDL_GetKeyboardState(NULL);

#ifdef _DEBUG
	// load font
    font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf", 16);

	// make debug values menu
	mnu_debugvals = SGUI_Menu_new(renderer, THEME_DEBUG.menu);
    SGUI_Label_new(&lbl_velocity_x, &mnu_debugvals, font, THEME_DEBUG.label);
    SGUI_Label_new(&lbl_velocity_x_val, &mnu_debugvals, font, THEME_DEBUG.label);
    SGUI_Label_new(&lbl_velocity_y, &mnu_debugvals, font, THEME_DEBUG.label);
    SGUI_Label_new(&lbl_velocity_y_val, &mnu_debugvals, font, THEME_DEBUG.label);
    SGUI_Label_new(&lbl_pos_x, &mnu_debugvals, font, THEME_DEBUG.label);
    SGUI_Label_new(&lbl_pos_x_val, &mnu_debugvals, font, THEME_DEBUG.label);
    SGUI_Label_new(&lbl_pos_y, &mnu_debugvals, font, THEME_DEBUG.label);
    SGUI_Label_new(&lbl_pos_y_val, &mnu_debugvals, font, THEME_DEBUG.label);
    SGUI_Label_new(&lbl_grounded, &mnu_debugvals, font, THEME_DEBUG.label);
    SGUI_Label_new(&lbl_grounded_val, &mnu_debugvals, font, THEME_DEBUG.label);

    // define menu
    mnu_debugvals.rect.x = 0;
    mnu_debugvals.rect.y = 0;
    mnu_debugvals.rect.w = cfg->gfx_window_w;
    mnu_debugvals.rect.h = cfg->gfx_window_h;

    SM_String_copy_cstr(&lbl_velocity_x.text, "vel_x:");
    SGUI_Label_update_sprite(&lbl_velocity_x);
    lbl_velocity_x.rect.w = lbl_velocity_x.sprite.surface->w;
    lbl_velocity_x.rect.h = lbl_velocity_x.sprite.surface->h;
    lbl_velocity_x.rect.x = 0;
    lbl_velocity_x.rect.y = 0;

    SM_String_copy_cstr(&lbl_velocity_x_val.text, "0");
    SGUI_Label_update_sprite(&lbl_velocity_x_val);
    lbl_velocity_x_val.rect.w = lbl_velocity_x_val.sprite.surface->w;
    lbl_velocity_x_val.rect.h = lbl_velocity_x_val.sprite.surface->h;
    lbl_velocity_x_val.rect.x = lbl_velocity_x.rect.x + lbl_velocity_x.rect.w + 10;
    lbl_velocity_x_val.rect.y = lbl_velocity_x.rect.y;

    SM_String_copy_cstr(&lbl_velocity_y.text, "vel_y:");
    SGUI_Label_update_sprite(&lbl_velocity_y);
    lbl_velocity_y.rect.w = lbl_velocity_y.sprite.surface->w;
    lbl_velocity_y.rect.h = lbl_velocity_y.sprite.surface->h;
    lbl_velocity_y.rect.x = lbl_velocity_x.rect.x;
    lbl_velocity_y.rect.y = lbl_velocity_x.rect.y + lbl_velocity_x.rect.h + 10;

    SM_String_copy_cstr(&lbl_velocity_y_val.text, "0");
    SGUI_Label_update_sprite(&lbl_velocity_y_val);
    lbl_velocity_y_val.rect.w = lbl_velocity_y_val.sprite.surface->w;
    lbl_velocity_y_val.rect.h = lbl_velocity_y_val.sprite.surface->h;
    lbl_velocity_y_val.rect.x = lbl_velocity_y.rect.x + lbl_velocity_y.rect.w + 10;
    lbl_velocity_y_val.rect.y = lbl_velocity_y.rect.y;

	SM_String_copy_cstr(&lbl_pos_x.text, "pos_x:");
    SGUI_Label_update_sprite(&lbl_pos_x);
    lbl_pos_x.rect.w = lbl_pos_x.sprite.surface->w;
    lbl_pos_x.rect.h = lbl_pos_x.sprite.surface->h;
    lbl_pos_x.rect.x = lbl_velocity_x.rect.x;
    lbl_pos_x.rect.y = lbl_velocity_y_val.rect.y + lbl_velocity_y_val.rect.h + 10;

    SM_String_copy_cstr(&lbl_pos_x_val.text, "0");
    SGUI_Label_update_sprite(&lbl_pos_x_val);
    lbl_pos_x_val.rect.w = lbl_pos_x_val.sprite.surface->w;
    lbl_pos_x_val.rect.h = lbl_pos_x_val.sprite.surface->h;
    lbl_pos_x_val.rect.x = lbl_pos_x.rect.x + lbl_pos_x.rect.w + 10;
    lbl_pos_x_val.rect.y = lbl_pos_x.rect.y;

    SM_String_copy_cstr(&lbl_pos_y.text, "pos_y:");
    SGUI_Label_update_sprite(&lbl_pos_y);
    lbl_pos_y.rect.w = lbl_pos_y.sprite.surface->w;
    lbl_pos_y.rect.h = lbl_pos_y.sprite.surface->h;
    lbl_pos_y.rect.x = lbl_velocity_x.rect.x;
    lbl_pos_y.rect.y = lbl_pos_x_val.rect.y + lbl_pos_x_val.rect.h + 10;

    SM_String_copy_cstr(&lbl_pos_y_val.text, "0");
    SGUI_Label_update_sprite(&lbl_pos_y_val);
    lbl_pos_y_val.rect.w = lbl_pos_y_val.sprite.surface->w;
    lbl_pos_y_val.rect.h = lbl_pos_y_val.sprite.surface->h;
    lbl_pos_y_val.rect.x = lbl_pos_x.rect.x + lbl_pos_x.rect.w + 10;
    lbl_pos_y_val.rect.y = lbl_pos_y.rect.y;

    SM_String_copy_cstr(&lbl_grounded.text, "grnd:");
    SGUI_Label_update_sprite(&lbl_grounded);
    lbl_grounded.rect.w = lbl_grounded.sprite.surface->w;
    lbl_grounded.rect.h = lbl_grounded.sprite.surface->h;
    lbl_grounded.rect.x = lbl_velocity_x.rect.x;
    lbl_grounded.rect.y = lbl_pos_y_val.rect.y + lbl_pos_y_val.rect.h + 10;

    SM_String_copy_cstr(&lbl_grounded_val.text, "0");
    SGUI_Label_update_sprite(&lbl_grounded_val);
    lbl_grounded_val.rect.w = lbl_grounded_val.sprite.surface->w;
    lbl_grounded_val.rect.h = lbl_grounded_val.sprite.surface->h;
    lbl_grounded_val.rect.x = lbl_grounded.rect.x + lbl_grounded.rect.w + 10;
    lbl_grounded_val.rect.y = lbl_grounded.rect.y;
#endif

	// mainloop
    while (game_active)
    {
		ts1 = now();

		// process events
		while (SDL_PollEvent(&event))
		{
			// app events
			switch (event.type)
			{
			case SDL_QUIT:
				game_active = false;
                break;
			}
		}

		// handle keyboard
		if (kbd[SDL_SCANCODE_A])
		{
			player->velocity_x -= DATA_ENTITIES[E_PLAYER].acceleration * delta;

			if (player->velocity_x < DATA_ENTITIES[E_PLAYER].max_velocity * -1)
				player->velocity_x = DATA_ENTITIES[E_PLAYER].max_velocity * -1;
		}

		if (kbd[SDL_SCANCODE_D])
		{
			player->velocity_x += DATA_ENTITIES[E_PLAYER].acceleration * delta;

			if (player->velocity_x > DATA_ENTITIES[E_PLAYER].max_velocity)
				player->velocity_x = DATA_ENTITIES[E_PLAYER].max_velocity;
		}

		if (kbd[SDL_SCANCODE_SPACE])
		{
			if (player->grounded)
				player->velocity_y -= DATA_ENTITIES[E_PLAYER].jump_velocity;
		}

		// gravity
		player->velocity_y += ENTITY_GRAVITY * delta;

		// apply walking friction or stop at velocity threshold
		if (player->grounded)
		{
			if (player->velocity_x > ENTITY_VELOCITY_THRESHOLD)
				player->velocity_x -= DATA_ENTITIES[E_PLAYER].decceleration * delta;

			else if (player->velocity_x < (ENTITY_VELOCITY_THRESHOLD * -1.0f))
				player->velocity_x += DATA_ENTITIES[E_PLAYER].decceleration * delta;

			else
				player->velocity_x = 0.0f;
		}

		// movement proccessing
		if (player->velocity_x != 0.0f)
		{
			x_step = player->velocity_x * delta;
			WldEntity_move_x(player, x_step, &world);
		}

		if (player->velocity_y != 0.0f)
		{
			y_step = player->velocity_y * delta;
			WldEntity_move_y(player, y_step, &world);
		}

#ifdef _DEBUG
        sprintf(lbl_velocity_x_val.text.str, "%f", player->velocity_x);
        lbl_velocity_x_val.text.len = strlen(lbl_velocity_x_val.text.str);
        SGUI_Label_update_sprite(&lbl_velocity_x_val);
        lbl_velocity_x_val.rect.w = lbl_velocity_x_val.sprite.surface->w;
        lbl_velocity_x_val.rect.h = lbl_velocity_x_val.sprite.surface->h;

        sprintf(lbl_velocity_y_val.text.str, "%f", player->velocity_y);
        sprintf(lbl_velocity_y_val.text.str, "%f", player->rect.y);
        lbl_velocity_y_val.text.len = strlen(lbl_velocity_y_val.text.str);
        SGUI_Label_update_sprite(&lbl_velocity_y_val);
        lbl_velocity_y_val.rect.w = lbl_velocity_y_val.sprite.surface->w;
        lbl_velocity_y_val.rect.h = lbl_velocity_y_val.sprite.surface->h;

        sprintf(lbl_pos_x_val.text.str, "%f", player->rect.x);
        lbl_pos_x_val.text.len = strlen(lbl_pos_x_val.text.str);
        SGUI_Label_update_sprite(&lbl_pos_x_val);
        lbl_pos_x_val.rect.w = lbl_pos_x_val.sprite.surface->w;
        lbl_pos_x_val.rect.h = lbl_pos_x_val.sprite.surface->h;

        sprintf(lbl_pos_y_val.text.str, "%f", player->rect.y);
        lbl_pos_y_val.text.len = strlen(lbl_pos_y_val.text.str);
        SGUI_Label_update_sprite(&lbl_pos_y_val);
        lbl_pos_y_val.rect.w = lbl_pos_y_val.sprite.surface->w;
        lbl_pos_y_val.rect.h = lbl_pos_y_val.sprite.surface->h;

        sprintf(lbl_grounded_val.text.str, "%i", player->grounded);
        lbl_grounded_val.text.len = strlen(lbl_grounded_val.text.str);
        SGUI_Label_update_sprite(&lbl_grounded_val);
        lbl_grounded_val.rect.w = lbl_grounded_val.sprite.surface->w;
        lbl_grounded_val.rect.h = lbl_grounded_val.sprite.surface->h;
#endif

		// update viewport
		viewport.x = (player->rect.x + player->rect.w) - (viewport.w / 2);
		viewport.y = (player->rect.y + player->rect.h) - (viewport.h / 2);

		if (viewport.x < 0)
			viewport.x = 0;

		else if ((viewport.x + viewport.w) >= (int) (world.width * BLOCK_SIZE))
			viewport.x = (world.width * BLOCK_SIZE) - viewport.w;

		if (viewport.y < 0)
			viewport.y = 0;

		else if ((viewport.y + viewport.h) >= (int) (world.height * BLOCK_SIZE))
			viewport.y = (world.height * BLOCK_SIZE) - viewport.h;

		// update block draw range
		wld_draw_pts[0].x = (viewport.x / BLOCK_SIZE);
		wld_draw_pts[0].y = (viewport.y / BLOCK_SIZE);

		wld_draw_pts[1].x = ((viewport.x + viewport.w) / BLOCK_SIZE);
		wld_draw_pts[1].y = ((viewport.y + viewport.h) / BLOCK_SIZE);

		// set sdl viewport
		viewport.x *= -1;
		viewport.y *= -1;
		SDL_RenderSetViewport(renderer, &viewport);

		// draw background
    	SDL_SetRenderDrawColor(renderer, 155, 219, 245, 255);
    	SDL_RenderClear(renderer);

		// draw walls and blocks
		for (int x = wld_draw_pts[0].x; x < wld_draw_pts[1].x; x++)
		{
    		for (int y = wld_draw_pts[0].y; y < wld_draw_pts[1].y; y++)
    		{
				temp.x = x * BLOCK_SIZE;
				temp.y = y * BLOCK_SIZE;
				temp.w = BLOCK_SIZE;
				temp.h = BLOCK_SIZE;

				SDL_RenderCopy(
					renderer,
					world.wall_textures[x][y],
					NULL,
					&temp);

				SDL_RenderCopy(
					renderer,
					world.block_textures[x][y],
					NULL,
					&temp);
			}
		}

		// draw entities
		for (size_t i = 0; i < world.ent_count; i++)
		{
            switch (world.entities[i].id)
            {
            case E_NONE:
            case E_PLAYER:
				break;
            }
		}

		// draw player
		temp.x = player->rect.x;
		temp.y = player->rect.y;
		temp.w = player->rect.w;
		temp.h = player->rect.h;

		SDL_RenderCopy(
			renderer,
			spr_ents[E_PLAYER].texture,
			NULL,
			&temp);

#ifdef _DEBUG
		// and optical collision box (not actual hitbox)
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_RenderDrawRect(renderer, &player->box);

		// draw debug menu (deactivate viewport for it)
		SDL_RenderSetViewport(renderer, NULL);
		SGUI_Menu_draw(&mnu_debugvals);
		SDL_RenderSetViewport(renderer, &viewport);
#endif

		// show drawn image
		SDL_RenderPresent(renderer);

		// timestamp and delta
		ts2 = now();
		delta = ts2 - ts1;
		delta *= TIMESCALE;
	}

	game_clear:

	// reset viewport
	SDL_RenderSetViewport(renderer, NULL);

	// clear strings
	SM_String_clear(&msg);

	// clear sprites
    for (uint_fast32_t i = 1; i <= B_LAST; i++)
    {
    	SGUI_Sprite_clear(&spr_blocks[i]);
    	SGUI_Sprite_clear(&spr_walls[i]);
    }

    for (uint_fast32_t i = 1; i <= E_LAST; i++)
    {
    	SGUI_Sprite_clear(&spr_ents[i]);
	}

	// clear world
	World_clear(&world);
}
