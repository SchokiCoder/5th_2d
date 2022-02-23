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
#include "definitions/def_app.h"
#include "definitions/def_files.h"
#include "world.h"
#include "sprite.h"

typedef struct BlockSprites {
	Sprite dirt;
	Sprite stone;
} BlockSprites ;

int main( int argc, char *argv[] ) {
	World world;
	SDL_Window *window;
	SDL_Renderer *renderer;
	BlockSprites spr_blocks;
	bool active = true;
	SDL_Rect temp;
	SDL_Event event;

	// if no world name given, stop
	if (argc < 2) {
		printf("Usage: %s WORLD_PATH\n", argv[0]);
		return 0;
	}

	// open world
	world = World_read(argv[1]);

    // init SDL
    SDL_Init(SDL_INIT_VIDEO);

    // create window and renderer
    window = SDL_CreateWindow(
    	APP_NAME,
    	SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    	640, 480,
    	SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, 0);

    // load sprites
    spr_blocks.dirt = Sprite_from_file(renderer, PATH_TEXTURE_BLOCK_DIRT);
    spr_blocks.stone = Sprite_from_file(renderer, PATH_TEXTURE_BLOCK_STONE);

    // map textures
    for (uint32_t x = 0; x < WORLD_MAX_WIDTH; x++) {
    	for (uint32_t y = 0; y < WORLD_MAX_HEIGHT; y++) {

    		// to blocks
    		switch (world.blocks[x][y]) {
    		case B_NONE:
    			world.block_textures[x][y] = NULL;
    			break;

			case B_DIRT:
    			world.block_textures[x][y] = spr_blocks.dirt.texture;
    			break;

			case B_STONE:
    			world.block_textures[x][y] = spr_blocks.stone.texture;
    			break;
    		}

    		// to walls
    		switch (world.walls[x][y]) {
    		case B_NONE:
    			world.wall_textures[x][y] = NULL;
    			break;

			case B_DIRT:
    			world.wall_textures[x][y] = spr_blocks.dirt.texture;
    			break;

			case B_STONE:
    			world.wall_textures[x][y] = spr_blocks.stone.texture;
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

		// draw
		SDL_RenderPresent(renderer);

		// handle events
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				active = false;
                break;
			}
		}
    }

    // quit SDL
    SDL_Quit();

	return 0;
}

/* TODO delete as soon as testing world io done
World in = {
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
World out;

World_write(&in);
out = World_read("test");

World_print(&in);
World_print(&out);
return 0;
*/


/* TODO delete as soon as testing world io done
World in = {
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
	}
};
World out;

World_write(&in);
out = World_read("test");

World_print(&in);
World_print(&out);
return 0;
*/
