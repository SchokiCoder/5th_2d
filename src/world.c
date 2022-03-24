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

#include <stdio.h>
#include <stdint.h>
#include <SM_log.h>
#include "entity.h"
#include "world.h"

void write_world( World *world )
{
	FILE *f;

	// open file
	f = fopen(world->world_name, "w");

	// if file did not open, set flag and stop
	if (f == NULL) {
		SM_log_err("World could not be read.");
		world->invalid = true;
		return;
	}

	// write blocks
    for (uint32_t x = 0; x < WORLD_MAX_WIDTH; x++) {
    	fwrite(world->blocks[x], sizeof(world->blocks[x][0]), WORLD_MAX_WIDTH, f);
    }

    // write walls
    for (uint32_t x = 0; x < WORLD_MAX_WIDTH; x++) {
    	fwrite(world->walls[x], sizeof(world->walls[x][0]), WORLD_MAX_WIDTH, f);
    }

    // write entities
    for (uint32_t i = 0; i < WORLD_MAX_ENTITIES; i++) {
    	fwrite(&world->entities[i].type, sizeof(world->entities[i].type), 1, f);
    	fwrite(&world->entities[i].rect.x, sizeof(world->entities[i].rect.x), 1, f);
    	fwrite(&world->entities[i].rect.y, sizeof(world->entities[i].rect.y), 1, f);
    	fwrite(&world->entities[i].rect.w, sizeof(world->entities[i].rect.w), 1, f);
    	fwrite(&world->entities[i].rect.h, sizeof(world->entities[i].rect.h), 1, f);
    }

    // close file
    fclose(f);
}

World read_world( const char *world_name )
{
	World result;
	FILE *f;

	// open file
	f = fopen(world_name, "r");

	// if file did not open, set flag and stop
	if (f == NULL) {
		result.invalid = true;
		return result;
	}

	// set name
	strcpy(result.world_name, world_name);

	// read blocks
	for (uint32_t x = 0; x < WORLD_MAX_WIDTH; x++) {
		fread(result.blocks[x], sizeof(result.blocks[x][0]), WORLD_MAX_WIDTH, f);
	}

	// read walls
	for (uint32_t x = 0; x < WORLD_MAX_WIDTH; x++) {
		fread(result.walls[x], sizeof(result.walls[x][0]), WORLD_MAX_WIDTH, f);
	}

	// read entitites
	for (uint32_t i = 0; i < WORLD_MAX_ENTITIES; i++) {
		fread(&result.entities[i].type, sizeof(result.entities[i].type), 1, f);
		fread(&result.entities[i].rect.x, sizeof(result.entities[i].rect.x), 1, f);
		fread(&result.entities[i].rect.y, sizeof(result.entities[i].rect.y), 1, f);
		fread(&result.entities[i].rect.w, sizeof(result.entities[i].rect.w), 1, f);
		fread(&result.entities[i].rect.h, sizeof(result.entities[i].rect.h), 1, f);
	}

	// close file, return
	fclose(f);
	return result;
}
