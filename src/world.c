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
#include "path.h"
#include "entity.h"
#include "world.h"

void World_allocate( World *world )
{
	world->blocks = malloc(world->width * sizeof(Block*));
	world->walls = malloc(world->width * sizeof(Block*));
	world->block_textures = malloc(world->width * sizeof(SDL_Texture**));
	world->wall_textures = malloc(world->width * sizeof(SDL_Texture**));

	for (size_t i = 0; i < world->width; i++)
	{
		world->blocks[i] = malloc(world->height * sizeof(Block));
		world->walls[i] = malloc(world->height * sizeof(Block));
		world->block_textures[i] = malloc(world->height * sizeof(SDL_Texture*));
		world->wall_textures[i] = malloc(world->height * sizeof(SDL_Texture*));
	}
}

World World_new( const char *world_name, const size_t width, const size_t height )
{
	World world = {
		.invalid = false,
		.width = width,
		.height = height,
	};

	// set values
	strcpy(world.world_name, world_name);
	World_allocate(&world);

	return world;
}

World World_from_file( const char *world_name )
{
	SM_String filepath = SM_String_new(8);
	World world = {
		.invalid = false
	};
	FILE *f;

	// get path
	if (get_world_path(&filepath) != 0)
	{
		world.invalid = true;
		return world;
	}

	SM_String_append_cstr(&filepath, world_name);
	SM_String_append_cstr(&filepath, ".");
	SM_String_append_cstr(&filepath, FILETYPE_WORLD);

	// open file
	f = fopen(filepath.str, "r");

	// if file did not open, set flag and stop
	if (f == NULL)
	{
		world.invalid = true;
		return world;
	}

	// set name
	strcpy(world.world_name, world_name);

	// read header
	fread(&world.width, sizeof(world.width), 1, f);
	fread(&world.height, sizeof(world.height), 1, f);

	// allocate for blocks
	World_allocate(&world);

	// read blocks
	for (uint32_t x = 0; x < world.width; x++)
		fread(world.blocks[x], sizeof(world.blocks[x][0]), world.height, f);

	// read walls
	for (uint32_t x = 0; x < world.width; x++)
		fread(world.walls[x], sizeof(world.walls[x][0]), world.height, f);

	// read entitites
	for (uint32_t i = 0; i < WORLD_MAX_ENTITIES; i++)
	{
		fread(&world.entities[i].type, sizeof(world.entities[i].type), 1, f);
		fread(&world.entities[i].rect.x, sizeof(world.entities[i].rect.x), 1, f);
		fread(&world.entities[i].rect.y, sizeof(world.entities[i].rect.y), 1, f);
		fread(&world.entities[i].rect.w, sizeof(world.entities[i].rect.w), 1, f);
		fread(&world.entities[i].rect.h, sizeof(world.entities[i].rect.h), 1, f);
	}

	SM_String_clear(&filepath);
	fclose(f);
	return world;
}

void World_write( World *world )
{
	SM_String filepath = SM_String_new(8);
	FILE *f;

	// get path
	if (get_world_path(&filepath) != 0)
	{
		world->invalid = true;
		return;
	}

	SM_String_append_cstr(&filepath, world->world_name);
	SM_String_append_cstr(&filepath, ".");
	SM_String_append_cstr(&filepath, FILETYPE_WORLD);

	// open file
	f = fopen(filepath.str, "w");

	// if file did not open, set flag and stop
	if (f == NULL)
	{
		SM_log_err("World could not be written.");
		world->invalid = true;
		return;
	}

	// write header
	fwrite(&world->width, sizeof(world->width), 1, f);
	fwrite(&world->height, sizeof(world->height), 1, f);

	// write blocks
    for (uint32_t x = 0; x < world->width; x++)
		fwrite(world->blocks[x], sizeof(world->blocks[x][0]), world->height, f);

    // write walls
    for (uint32_t x = 0; x < world->width; x++)
		fwrite(world->walls[x], sizeof(world->walls[x][0]), world->height, f);

    // write entities
    for (uint32_t i = 0; i < WORLD_MAX_ENTITIES; i++)
    {
    	fwrite(&world->entities[i].type, sizeof(world->entities[i].type), 1, f);
    	fwrite(&world->entities[i].rect.x, sizeof(world->entities[i].rect.x), 1, f);
    	fwrite(&world->entities[i].rect.y, sizeof(world->entities[i].rect.y), 1, f);
    	fwrite(&world->entities[i].rect.w, sizeof(world->entities[i].rect.w), 1, f);
    	fwrite(&world->entities[i].rect.h, sizeof(world->entities[i].rect.h), 1, f);
    }

    SM_String_clear(&filepath);
    fclose(f);
}

void World_clear( World *world )
{
	for (size_t i = 0; i < world->width; i++)
	{
		free(world->blocks[i]);
		free(world->walls[i]);
		free(world->block_textures[i]);
		free(world->wall_textures[i]);
	}

	free(world->blocks);
	free(world->walls);
	free(world->block_textures);
	free(world->wall_textures);
}
