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
#include "world.h"
#include "entity.h"

void World_print( const World *self ) {
	// print name
	printf("world_name: %s\n", self->world_name);

	// print blocks
	for (uint32_t x = 0; x < WORLD_MAX_WIDTH; x++) {
		for (uint32_t y = 0; y < WORLD_MAX_HEIGHT; y++) {
			printf("%u", self->blocks[x][y]);
		}
		printf("\n");
	}
}

void World_write( const World *self ) {
	FILE *f;

	// open file
	f = fopen(self->world_name, "w");

	// write blocks
    for (uint32_t x = 0; x < WORLD_MAX_WIDTH; x++) {
    	fwrite(self->blocks[x], sizeof(self->blocks[x][0]), WORLD_MAX_WIDTH, f);
    }

    // write walls
    for (uint32_t x = 0; x < WORLD_MAX_WIDTH; x++) {
    	fwrite(self->walls[x], sizeof(self->walls[x][0]), WORLD_MAX_WIDTH, f);
    }

    // write entities
    for (uint32_t i = 0; i < WORLD_MAX_ENTITIES; i++) {
    	fwrite(&self->entities[i].type, sizeof(self->entities[i].type), 1, f);
    	fwrite(&self->entities[i].pos.x, sizeof(self->entities[i].pos.x), 1, f);
    	fwrite(&self->entities[i].pos.y, sizeof(self->entities[i].pos.y), 1, f);
    	fwrite(&self->entities[i].pos.w, sizeof(self->entities[i].pos.w), 1, f);
    	fwrite(&self->entities[i].pos.h, sizeof(self->entities[i].pos.h), 1, f);
    }

    // close file
    fclose(f);
}

World World_read( const char *world_name ) {
	World result;
	FILE *f;

	// open file
	f = fopen(world_name, "r");

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
		fread(&result.entities[i].pos.x, sizeof(result.entities[i].pos.x), 1, f);
		fread(&result.entities[i].pos.y, sizeof(result.entities[i].pos.y), 1, f);
		fread(&result.entities[i].pos.w, sizeof(result.entities[i].pos.w), 1, f);
		fread(&result.entities[i].pos.h, sizeof(result.entities[i].pos.h), 1, f);
	}

	// close file, return
	fclose(f);
	return result;
}
