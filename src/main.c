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
#include <time.h>
#include <SDL.h>
#include "app.h"
#include "motd.h"
#include "log.h"
#include "world.h"
#include "sprite.h"
#include "player.h"

#ifdef _WIN32
# define SLASH "\\"
#else
# define SLASH "/"
#endif

#define PATH_TEXTURES_BLOCKS PATH_TEXTURES "blocks"
#define PATH_TEXTURES_ENTITIES PATH_TEXTURES "entities"
static const char PATH_TEXTURE_BLOCK_DIRT[] = PATH_TEXTURES_BLOCKS SLASH "dirt.png";
static const char PATH_TEXTURE_BLOCK_STONE[] = PATH_TEXTURES_BLOCKS SLASH "stone.png";
static const char PATH_TEXTURE_ENT_PLAYER[] = PATH_TEXTURES_ENTITIES SLASH "player.png";

void gen_demo_vertical( void ) {
	World out = {
		.world_name = "test",
		.blocks = {
			[5] = {
				[0] = B_DIRT,
				[1] = B_DIRT,
				[2] = B_DIRT,
				[3] = B_DIRT,
				[4] = B_DIRT,
				[5] = B_DIRT
			},
			[6] = {
				[1] = B_STONE,
				[2] = B_STONE,
				[3] = B_STONE,
				[4] = B_STONE,
				[5] = B_STONE,
				[6] = B_STONE,
				[7] = B_STONE,
			}
		},
		.walls = {
			[3] = {
				[0] = B_DIRT,
				[1] = B_DIRT,
				[2] = B_DIRT,
				[3] = B_DIRT
			},
			[4] = {
				[0] = B_DIRT,
				[1] = B_DIRT,
				[2] = B_DIRT,
				[3] = B_DIRT,
				[4] = B_DIRT
			}
		}
	};

	write_world(&out);
}

void gen_demo_horizontal( void ) {
	World out = {
		.world_name = "test",
		.blocks = {
			[0] = {
				[0] = B_STONE,
				[1] = B_STONE,
				[5] = B_DIRT
			},
			[1] = {
				[5] = B_DIRT,
				[6] = B_STONE
			},
			[2] = {
				[5] = B_DIRT,
				[6] = B_STONE
			},
			[3] = {
				[5] = B_DIRT,
				[6] = B_STONE
			},
			[4] = {
				[5] = B_DIRT,
				[6] = B_STONE
			},
			[5] = {
				[5] = B_DIRT,
				[6] = B_STONE
			},
			[6] = {
				[6] = B_STONE
			},
			[7] = {
				[6] = B_STONE
			},
			[10] = {
				[0] = B_STONE,
				[1] = B_STONE
			}
		},
		.walls = {
			[0] = {
				[3] = B_DIRT,
				[4] = B_DIRT
			},
			[1] = {
				[3] = B_DIRT,
				[4] = B_DIRT
			},
			[2] = {
				[3] = B_DIRT,
				[4] = B_DIRT
			},
			[3] = {
				[3] = B_DIRT,
				[4] = B_DIRT
			},
			[4] = {
				[4] = B_DIRT
			}
		},
		.entities[0] = {
			.type = E_PLAYER,
			.rect = {
				.x = 2.0f * BLOCK_SIZE,
				.y = 1.0f * BLOCK_SIZE,
				.w = PLAYER_WIDTH,
				.h = PLAYER_HEIGHT
			}
		}
	};

	write_world(&out);
}

float now( void )
{
	return (float) clock() / (float) CLOCKS_PER_SEC;
}

int main( int argc, char *argv[] )
{
	char window_title[86];
	World world = {.invalid = false};
	Player player = {.velocity_x = 0.0f, .velocity_y = 0.0f};
	const uint8_t *kbd;
	float ts1, ts2, delta = 0.0f;
	float x_step = 0.0f;
	float y_step = 0.0f;
	SDL_Window *window;
	SDL_Renderer *renderer;
	Sprite spr_blocks[NUM_BLOCK_TYPES];
	Sprite spr_walls[NUM_BLOCK_TYPES];
	Sprite spr_ents[NUM_ENT_TYPES];
	bool active = true;
	SDL_Rect temp;
	SDL_Event event;

	// open log file and check
	logfile = fopen(PATH_LOG, "a");

	if (logfile == NULL)
	{
		printf("Log file \"%s\" could not be opened.\nAbort.\n", PATH_LOG);
		return 1;
	}

	// if no world name given, stop
	if (argc < 2)
	{
		printf("Usage: %s WORLD_PATH\n", argv[0]);
		return 0;
	}

	// seed random generator
	srand(time(NULL));

	// open world and check
	world = read_world(argv[1]);

	if (world.invalid)
	{
		return 1;
	}

	// set player values
	player.rect.x = (float) world.entities[0].rect.x;
	player.rect.y = (float) world.entities[0].rect.y;
	player.rect.w = (float) world.entities[0].rect.w;
	player.rect.h = (float) world.entities[0].rect.h;

    // init SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
    	log_err("SDL could not initialize.");
    	return 1;
    }

    // create window title
    strcpy(window_title, APP_NAME);
    strcat(window_title, " ");
    strcat(window_title, MOTDS[rand() % sizeof(MOTDS) / sizeof(MOTDS[0])]);

    // create window and renderer
    window = SDL_CreateWindow(
    	window_title,
    	SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    	640, 480,
    	SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, 0);

    // check window and renderer
    if (window == NULL || renderer == NULL)
    {
    	log_err("SDL could not open a window and renderer.");
    	return 1;
    }

    // load block sprites
    spr_blocks[B_NONE].surface = NULL;
    spr_blocks[B_NONE].texture = NULL;
    spr_blocks[B_NONE].invalid = false;
    spr_blocks[B_DIRT] = sprite_from_file(renderer, PATH_TEXTURE_BLOCK_DIRT);
    spr_blocks[B_STONE] = sprite_from_file(renderer, PATH_TEXTURE_BLOCK_STONE);

    // check block sprites
    for (uint32_t i = 1; i < NUM_BLOCK_TYPES; i++)
    {
    	if (spr_blocks[i].invalid)
    		return 1;
	}

	// create wall sprites
	temp.x = 0;
	temp.y = 0;
	temp.w = BLOCK_SIZE;
	temp.h = BLOCK_SIZE;

	spr_walls[0].surface = NULL;
	spr_walls[0].texture = NULL;
	spr_walls[0].invalid = false;

    for (uint32_t i = 1; i < NUM_BLOCK_TYPES; i++)
    {
    	// copy block surface, modify, create texture
    	spr_walls[i].surface = SDL_ConvertSurface(spr_blocks[i].surface, spr_blocks[i].surface->format, 0);
		SDL_SetSurfaceColorMod(spr_walls[i].surface, 175, 175, 175);
    	sprite_create_texture(&spr_walls[i], renderer, "generated wall sprite");
    }

    // check wall sprites
    for (uint32_t i = 1; i < NUM_BLOCK_TYPES; i++)
    {
    	if (spr_walls[i].invalid)
    		return 1;
	}

	// load ent sprites
	spr_ents[E_NONE].surface = NULL;
	spr_ents[E_NONE].texture = NULL;
	spr_ents[E_NONE].invalid = false;
	spr_ents[E_PLAYER] = sprite_from_file(renderer, PATH_TEXTURE_ENT_PLAYER);

	// check ent sprites
	for (uint32_t i = 1; i < NUM_ENT_TYPES; i++)
    {
		if (spr_ents[i].invalid)
		{
			return 1;
		}
	}

    // map textures
    for (uint32_t x = 0; x < WORLD_MAX_WIDTH; x++)
    {
    	for (uint32_t y = 0; y < WORLD_MAX_HEIGHT; y++)
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

		// handle events
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			// window close
			case SDL_QUIT:
				active = false;
                break;
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
		for (uint32_t x = 0; x < WORLD_MAX_WIDTH; x++)
		{
    		for (uint32_t y = 0; y < WORLD_MAX_HEIGHT; y++)
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
		for (uint32_t i = 0; i < WORLD_MAX_ENTITIES; i++)
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

    // clear sprites
    for (uint32_t i = 1; i < NUM_BLOCK_TYPES; i++)
    {
    	clear_sprite(&spr_blocks[i]);
    	clear_sprite(&spr_walls[i]);
    }

    for (uint32_t i = 1; i < NUM_ENT_TYPES; i++)
    {
    	clear_sprite(&spr_ents[i]);
	}

    // quit SDL
    SDL_Quit();

    //close log file
    fclose(logfile);

	return 0;
}
