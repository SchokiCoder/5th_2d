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

	world->entities = malloc(world->ent_count * sizeof(WldEntity));
}

World World_new( const char *world_name, const size_t width, const size_t height )
{
	World world = {
		.invalid = false,
		.world_name = SM_String_from(world_name),
		.width = width,
		.height = height,
		.ent_count = 1,
	};

	// set values
	World_allocate(&world);
	world.entities[0].id = E_PLAYER;
	world.entities[0].rect.x = 0.0f;
	world.entities[0].rect.y = 0.0f;
	world.entities[0].rect.w = DATA_ENTITIES[E_PLAYER].width;
	world.entities[0].rect.h = DATA_ENTITIES[E_PLAYER].height;
	world.entities[0].grounded = false;
	world.entities[0].velocity_x = 0.0f;
	world.entities[0].velocity_y = 0.0f;

	return world;
}

World World_from_file( const char *world_name )
{
	SM_String msg = SM_String_new(8);
	SM_String filepath = SM_String_new(8);
	World world = {
		.invalid = false,
		.world_name = SM_String_from(world_name),
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

	// check file access
	switch (file_check_access(filepath.str))
	{
	case FA_NONE:
		SM_String_copy_cstr(&msg, "World ");
		SM_String_append_cstr(&msg, world_name);
		SM_String_append_cstr(&msg, " does not exist or permissions to read are missing.");
		SM_log_err(msg.str);

		world.invalid = true;
		return world;
		break;

	case FA_READ:
		SM_String_copy_cstr(&msg, "World ");
		SM_String_append_cstr(&msg, world_name);
		SM_String_append_cstr(&msg, " is not permitted to be written to.");
		SM_log_warn(msg.str);
		break;

	case FA_WRITE:
		break;
	}

	// open file
	f = fopen(filepath.str, "r");

	// read header
	fread(&world.width, sizeof(world.width), 1, f);
	fread(&world.height, sizeof(world.height), 1, f);
	fread(&world.ent_count, sizeof(world.ent_count), 1, f);

	// allocate for blocks
	World_allocate(&world);

	// read blocks
	for (size_t x = 0; x < world.width; x++)
		fread(world.blocks[x], sizeof(world.blocks[x][0]), world.height, f);

	// read walls
	for (size_t x = 0; x < world.width; x++)
		fread(world.walls[x], sizeof(world.walls[x][0]), world.height, f);

	// read entitites
	for (size_t i = 0; i < world.ent_count; i++)
	{
		fread(&world.entities[i].id, sizeof(world.entities[i].id), 1, f);
		fread(&world.entities[i].rect.x, sizeof(world.entities[i].rect.x), 1, f);
		fread(&world.entities[i].rect.y, sizeof(world.entities[i].rect.y), 1, f);
		fread(&world.entities[i].rect.w, sizeof(world.entities[i].rect.w), 1, f);
		fread(&world.entities[i].rect.h, sizeof(world.entities[i].rect.h), 1, f);
		fread(&world.entities[i].grounded, sizeof(world.entities[i].grounded), 1, f);
		fread(&world.entities[i].velocity_x, sizeof(world.entities[i].velocity_x), 1, f);
		fread(&world.entities[i].velocity_y, sizeof(world.entities[i].velocity_y), 1, f);
	}

	SM_String_clear(&msg);
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

	SM_String_append(&filepath, &world->world_name);
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
	fwrite(&world->ent_count, sizeof(world->ent_count), 1, f);

	// write blocks
    for (size_t x = 0; x < world->width; x++)
		fwrite(world->blocks[x], sizeof(world->blocks[x][0]), world->height, f);

    // write walls
    for (size_t x = 0; x < world->width; x++)
		fwrite(world->walls[x], sizeof(world->walls[x][0]), world->height, f);

    // write entities
    for (size_t i = 0; i < world->ent_count; i++)
    {
    	fwrite(&world->entities[i].id, sizeof(world->entities[i].id), 1, f);
    	fwrite(&world->entities[i].rect.x, sizeof(world->entities[i].rect.x), 1, f);
    	fwrite(&world->entities[i].rect.y, sizeof(world->entities[i].rect.y), 1, f);
    	fwrite(&world->entities[i].rect.w, sizeof(world->entities[i].rect.w), 1, f);
    	fwrite(&world->entities[i].rect.h, sizeof(world->entities[i].rect.h), 1, f);
    	fwrite(&world->entities[i].grounded, sizeof(world->entities[i].grounded), 1, f);
		fwrite(&world->entities[i].velocity_x, sizeof(world->entities[i].velocity_x), 1, f);
		fwrite(&world->entities[i].velocity_y, sizeof(world->entities[i].velocity_y), 1, f);
    }

    SM_String_clear(&filepath);
    fclose(f);
}

void World_clear( World *world )
{
	SM_String_clear(&world->world_name);

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

	free(world->entities);
}
