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
#define WORLD_MAX_ENTITIES 16

typedef struct World
{
	bool invalid;
	char world_name[WORLD_NAME_MAX_LEN];
	size_t width;
	size_t height;
    Block **blocks;					// actual terrain
    Block **walls;						// background terrain
    Entity entities[WORLD_MAX_ENTITIES];
    SDL_Texture ***block_textures;
    SDL_Texture ***wall_textures;
} World ;

World World_new( const char *world_name, const size_t width, const size_t height );

World World_from_file( const char *world_name );

void World_write( World *world );

void World_clear( World *world );

#endif /* WORLD_H */
