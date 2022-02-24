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
#include <SDL.h>
#include "app.h"
#include "log.h"
#include "world.h"
#include "sprite.h"

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
	World in;

	write_world(&out);
	in = read_world("test");

	print_world(&out);
	print_world(&in);
}

void gen_demo_horizontal( void ) {
	World out = {
		.world_name = "test",
		.blocks = {
			[0] = {
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
				.x = 2 * BLOCK_SIZE,
				.y = 1 * BLOCK_SIZE,
				.w = PLAYER_WIDTH,
				.h = PLAYER_HEIGHT
			}
		}
	};
	World in;

	write_world(&out);
	in = read_world("test");

	print_world(&out);
	print_world(&in);
}

int main( int argc, char *argv[] ) {
	World world = {.invalid = false};
	SDL_Window *window;
	SDL_Renderer *renderer;
	Sprite spr_block_dirt = {.invalid = false};
	Sprite spr_block_stone = {.invalid = false};
	Sprite spr_ent_player = {.invalid = false};
	bool active = true;
	SDL_Rect temp;
	SDL_Event event;

	// open log file and check
	logfile = fopen(PATH_LOG, "a");

	if (logfile == NULL) {
		printf("Log file \"%s\" could not be opened.\nAbort.\n", PATH_LOG);
		return 1;
	}

	// if no world name given, stop
	if (argc < 2) {
		printf("Usage: %s WORLD_PATH\n", argv[0]);
		return 0;
	}

	// open world and check
	world = read_world(argv[1]);

	if (world.invalid) {
		return 1;
	}

    // init SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    	log_err("SDL could not initialize.");
    	return 1;
    }

    // create window and renderer
    window = SDL_CreateWindow(
    	APP_NAME,
    	SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    	640, 480,
    	SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, 0);

    // check window and renderer
    if (window == NULL || renderer == NULL) {
    	log_err("SDL could not open a window and renderer.");
    	return 1;
    }

    // load sprites
    spr_block_dirt = sprite_from_file(renderer, PATH_TEXTURE_BLOCK_DIRT);
    spr_block_stone = sprite_from_file(renderer, PATH_TEXTURE_BLOCK_STONE);
    spr_ent_player = sprite_from_file(renderer, PATH_TEXTURE_ENT_PLAYER);

    // check sprites
    if (spr_block_dirt.invalid ||
    	spr_block_stone.invalid ||
    	spr_ent_player.invalid) {
    	return 1;
	}

    // map textures
    for (uint32_t x = 0; x < WORLD_MAX_WIDTH; x++) {
    	for (uint32_t y = 0; y < WORLD_MAX_HEIGHT; y++) {

    		// to blocks
    		switch (world.blocks[x][y]) {
    		case B_NONE:
    			world.block_textures[x][y] = NULL;
    			break;

			case B_DIRT:
    			world.block_textures[x][y] = spr_block_dirt.texture;
    			break;

			case B_STONE:
    			world.block_textures[x][y] = spr_block_stone.texture;
    			break;
    		}

    		// to walls
    		switch (world.walls[x][y]) {
    		case B_NONE:
    			world.wall_textures[x][y] = NULL;
    			break;

			case B_DIRT:
    			world.wall_textures[x][y] = spr_block_dirt.texture;
    			break;

			case B_STONE:
    			world.wall_textures[x][y] = spr_block_stone.texture;
    			break;
    		}
		}
	}

    // mainloop
    while (active) {
    	// draw background
    	SDL_SetRenderDrawColor(renderer, 155, 219, 245, 255);
    	SDL_RenderClear(renderer);

		// draw walls and blocks
		for (uint32_t x = 0; x < WORLD_MAX_WIDTH; x++) {
    		for (uint32_t y = 0; y < WORLD_MAX_HEIGHT; y++) {
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
		for (uint32_t i = 0; i < WORLD_MAX_ENTITIES; i++) {
            switch (world.entities[i].type) {
            case E_NONE:
            	break;

            case E_PLAYER:
            	SDL_RenderCopy(
            		renderer,
            		spr_ent_player.texture,
            		NULL,
            		&world.entities[i].rect);
				break;
            }
		}

		// show drawn image
		SDL_RenderPresent(renderer);

		// handle events
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				active = false;
                break;

			/*case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
				case SDLK_w:

                }
				break;*/
			}
		}
    }

    // clear sprites
    clear_sprite(&spr_block_dirt);
    clear_sprite(&spr_block_stone);
    clear_sprite(&spr_ent_player);

    // quit SDL
    SDL_Quit();

    //close log file
    fclose(logfile);

	return 0;
}
