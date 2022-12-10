/*
 * 2d_platformer
 * Copyright (C) 2022  Andy Frank Schoknecht
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not see
 * <https://www.gnu.org/licenses/old-licenses/gpl-2.0.html>.
 */

#include <stdio.h>
#include <stdint.h>
#include <SM_log.h>
#include "path.h"
#include "entity.h"
#include "world.h"

SG_World World_new(const size_t width, const size_t height)
{
	SG_World world = SG_World_new(BLOCK_SIZE, width, height, 2);

	// set values
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

SG_World World_from_file(const char *world_name)
{
	SG_World world;
	SM_String filepath = SM_String_new(8);

	// get path
	if (get_world_path(&filepath) != 0) {
		world.invalid = true;
		return world;
	}

	SM_String_append_cstr(&filepath, world_name);
	SM_String_append_cstr(&filepath, ".");
	SM_String_append_cstr(&filepath, FILETYPE_WORLD);

	// read
	world = SG_World_from_file(filepath.str);

	if (world.invalid) {
		SM_String msg = SM_String_new(16);
		SM_String_copy_cstr(&msg, "World \"");
		SM_String_append_cstr(&msg, world_name);
		SM_String_append_cstr(&msg, "\" could not be read.");
		SM_log_err(msg.str);
		SM_String_clear(&msg);
	}

	SM_String_clear(&filepath);
	return world;
}

void World_write(SG_World * world, const char *world_name)
{
	SM_String filepath = SM_String_new(8);

	// get path
	if (get_world_path(&filepath) != 0) {
		world->invalid = true;
		return;
	}

	SM_String_append_cstr(&filepath, world_name);
	SM_String_append_cstr(&filepath, ".");
	SM_String_append_cstr(&filepath, FILETYPE_WORLD);

	// write
	SG_World_write(world, filepath.str);

	if (world->invalid) {
		SM_String msg = SM_String_new(16);
		SM_String_copy_cstr(&msg, "World \"");
		SM_String_append_cstr(&msg, world_name);
		SM_String_append_cstr(&msg, "\" could not be read.");
		SM_log_err(msg.str);
		SM_String_clear(&msg);
	}

	SM_String_clear(&filepath);
}
