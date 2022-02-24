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

#ifndef WORLD_H
#define WORLD_H

#include <stdbool.h>
#include "entity.h"

#define WORLD_NAME_MAX_LEN 64
#define WORLD_MAX_WIDTH 16
#define WORLD_MAX_HEIGHT 16
#define WORLD_MAX_ENTITIES 16

typedef struct World {
	bool invalid;
	char world_name[WORLD_NAME_MAX_LEN];
    enum Block blocks[WORLD_MAX_WIDTH][WORLD_MAX_HEIGHT];	// actual terrain
    enum Block walls[WORLD_MAX_WIDTH][WORLD_MAX_HEIGHT];	// no collision and looks different
    Entity entities[WORLD_MAX_ENTITIES];
    SDL_Texture *block_textures[WORLD_MAX_WIDTH][WORLD_MAX_HEIGHT];
    SDL_Texture *wall_textures[WORLD_MAX_WIDTH][WORLD_MAX_HEIGHT];
} World ;

void print_world( const World* );

void write_world( World* );

World read_world( const char* );

#endif /* WORLD_H */
