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
#include <time.h>
#include "world.h"
#include "block.h"
#include "entity.h"
#include "player.h"
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


float now( void )
{
	return (float) clock() / (float) CLOCKS_PER_SEC;
}

void game_run( const char *path_world, SDL_Renderer *renderer )
{
	SM_String appendage;
	SM_String msg = SM_String_new(16);
	SGUI_Sprite spr_blocks[NUM_BLOCK_TYPES];
	SGUI_Sprite spr_walls[NUM_BLOCK_TYPES];
	SGUI_Sprite spr_ents[NUM_ENT_TYPES];
	World world = {.invalid = false};
	Player player = {.velocity_x = 0.0f, .velocity_y = 0.0f};
	SDL_Event event;
	SDL_Rect temp;
	const uint8_t *kbd;
	bool active = true;
	float ts1, ts2, delta = 0.0f;
	float x_step = 0.0f;
	float y_step = 0.0f;

	// open world and check
	world = read_world(path_world);

	if (world.invalid)
	{
		appendage = SM_String_contain("World ");
		SM_String_copy(&msg, &appendage);

		appendage = SM_String_contain(path_world);
		SM_String_append(&msg, &appendage);

		appendage = SM_String_contain(" is corrupt.");
		SM_String_append(&msg, &appendage);

		SM_log_err(msg.str);
		goto game_clear;
	}

	// set player values
	player.rect.x = (float) world.entities[0].rect.x;
	player.rect.y = (float) world.entities[0].rect.y;
	player.rect.w = (float) world.entities[0].rect.w;
	player.rect.h = (float) world.entities[0].rect.h;

	// load block sprites
    spr_blocks[B_NONE] = SGUI_Sprite_new();

    for (uint_fast32_t i = 1; i < NUM_BLOCK_TYPES; i++)
	{
		spr_blocks[i] = SGUI_Sprite_from_file(renderer, PATH_TEXTURES_BLOCKS[i - 1]);

		if (spr_blocks[i].invalid)
		{
			appendage = SM_String_contain("Sprite ");
			SM_String_copy(&msg, &appendage);

			appendage = SM_String_contain(PATH_TEXTURES_BLOCKS[i - 1]);
			SM_String_append(&msg, &appendage);

			appendage = SM_String_contain(" could not be loaded.");
			SM_String_append(&msg, &appendage);

			SM_log_err(msg.str);
			goto game_clear;
		}
	}

	// create wall sprites
	spr_walls[0] = SGUI_Sprite_new();

    for (uint_fast32_t i = 1; i < NUM_BLOCK_TYPES; i++)
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
	spr_ents[E_NONE] = SGUI_Sprite_new();

	for (uint_fast32_t i = 1; i < NUM_ENT_TYPES; i++)
	{
		spr_ents[i] = SGUI_Sprite_from_file(renderer, PATH_TEXTURES_ENTITIES[i - 1]);

		if (spr_ents[i].invalid)
		{
			appendage = SM_String_contain("Sprite ");
			SM_String_copy(&msg, &appendage);

			appendage = SM_String_contain(PATH_TEXTURES_ENTITIES[i - 1]);
			SM_String_append(&msg, &appendage);

			appendage = SM_String_contain(" could not be loaded.");
			SM_String_append(&msg, &appendage);

			SM_log_err(msg.str);
			goto game_clear;
		}
	}

    // map textures
    for (uint_fast32_t x = 0; x < WORLD_MAX_WIDTH; x++)
    {
    	for (uint_fast32_t y = 0; y < WORLD_MAX_HEIGHT; y++)
    	{
            world.block_textures[x][y] = spr_blocks[world.blocks[x][y]].texture;
            world.wall_textures[x][y] = spr_walls[world.walls[x][y]].texture;
		}
	}

	// set keyboard state pointer
	kbd = SDL_GetKeyboardState(NULL);

	// mainloop
    while (active)
    {
		ts1 = now();

		// process events
		while (SDL_PollEvent(&event))
		{
			// app events
			switch (event.type)
			{
			case SDL_QUIT:
				active = false;
                break;
			}
		}

		// handle keyboard
		if (kbd[SDL_SCANCODE_A])
		{
			player.velocity_x -= PLAYER_ACCELERATION * delta;

			if (player.velocity_x < PLAYER_MAX_VELOCITY * -1)
				player.velocity_x = PLAYER_MAX_VELOCITY * -1;
		}

		if (kbd[SDL_SCANCODE_D])
		{
			player.velocity_x += PLAYER_ACCELERATION * delta;

			if (player.velocity_x > PLAYER_MAX_VELOCITY)
				player.velocity_x = PLAYER_MAX_VELOCITY;
		}

		if (kbd[SDL_SCANCODE_SPACE])
		{
			if (player.grounded)
			{
				player.velocity_y -= PLAYER_JUMP_VELOCITY;
			}
		}

		// gravity
		player.velocity_y += GRAVITY * delta;

		// walking friction
		if (player.grounded)
		{
			player.velocity_x -= (player.velocity_x * PLAYER_WALKING_FRICTION * delta);
		}

		// movement proccessing
		x_step = player.velocity_x * delta;
		y_step = player.velocity_y * delta;

		move_player_x(&player, x_step, &world);
		move_player_y(&player, y_step, &world);

		// draw background
    	SDL_SetRenderDrawColor(renderer, 155, 219, 245, 255);
    	SDL_RenderClear(renderer);

		// draw walls and blocks
		for (uint_fast32_t x = 0; x < WORLD_MAX_WIDTH; x++)
		{
    		for (uint_fast32_t y = 0; y < WORLD_MAX_HEIGHT; y++)
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
		for (uint_fast32_t i = 0; i < WORLD_MAX_ENTITIES; i++)
		{
            switch (world.entities[i].type)
            {
            case E_NONE:
            case E_PLAYER:
				break;
            }
		}

		// draw player
		temp.x = (uint32_t) player.rect.x;
		temp.y = (uint32_t) player.rect.y;
		temp.w = (uint32_t) player.rect.w;
		temp.h = (uint32_t) player.rect.h;

		SDL_RenderCopy(
			renderer,
			spr_ents[E_PLAYER].texture,
			NULL,
			&temp);

		// show drawn image
		SDL_RenderPresent(renderer);

		// timestamp and delta
		ts2 = now();
		delta = ts2 - ts1;
	}

	game_clear:

	//clear strings
	SM_String_clear(&msg);

	// clear sprites
    for (uint_fast32_t i = 1; i < NUM_BLOCK_TYPES; i++)
    {
    	SGUI_Sprite_clear(&spr_blocks[i]);
    	SGUI_Sprite_clear(&spr_walls[i]);
    }

    for (uint_fast32_t i = 1; i < NUM_ENT_TYPES; i++)
    {
    	SGUI_Sprite_clear(&spr_ents[i]);
	}
}
