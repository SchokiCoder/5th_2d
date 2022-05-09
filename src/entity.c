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

#include <SG_world.h>
#include <SG_physics.h>
#include "entity.h"

/*
	player_pos: 		player.rect.x or y
	player_velocity:	player.velocity_x or y
*/
bool_t Entity_move( SG_Entity *ent, float *pos, float *velocity, float distance, SG_World *world )
{
	bool_t collision = FALSE;
	int_fast32_t x1, y1, x2, y2;
	SG_FRect block_hitbox;

	// set position
	*pos += distance;

	// calculate which blocks to check
	x1 = ent->rect.x / BLOCK_SIZE;
	y1 = ent->rect.y / BLOCK_SIZE;
	x2 = (ent->rect.x + ent->rect.w) / BLOCK_SIZE + 1;
	y2 = (ent->rect.y + ent->rect.h) / BLOCK_SIZE + 1;

	// check if within bounds
	if (x1 < 0)
		x1 = 0;

	if (x1 > (int_fast32_t) world->width - 1)
		x1 = (int_fast32_t) world->width - 1;

	if (y1 < 0)
		y1 = 0;

	if (y1 > (int_fast32_t) world->height - 1)
		y1 = (int_fast32_t) world->height - 1;

	if (x2 < 0)
		x2 = 0;

	if (x2 > (int_fast32_t) world->width - 1)
		x2 = (int_fast32_t) world->width - 1;

	if (y2 < 0)
		y2 = 0;

	if (y2 > (int_fast32_t) world->height - 1)
		y2 = (int_fast32_t) world->height - 1;

	// collision check for nearby blocks
	block_hitbox.w = BLOCK_SIZE;
	block_hitbox.h = BLOCK_SIZE;

	for (int_fast32_t x = x1; x <= x2; x++)
	{
		for (int_fast32_t y = y1; y <= y2; y++)
		{
			// if non-solid block here, skip
			if (world->blocks[x][y][0] == B_NONE)
				continue;

			block_hitbox.x = x * BLOCK_SIZE;
			block_hitbox.y = y * BLOCK_SIZE;

			// if collision
			if (SG_box_within_box(&ent->rect, &block_hitbox))
			{
				// flag, reset pos, kill velocity
				collision = TRUE;
				*pos -= distance;
				*velocity = 0.0f;
			}
		}
	}

	return collision;
}

void Entity_move_x( SG_Entity *ent, float x_distance, SG_World *world )
{
	Entity_move(ent, &ent->rect.x, &ent->velocity_x, x_distance, world);
}

void Entity_move_y( SG_Entity *ent, float y_distance, SG_World *world )
{
	bool_t collision;

	// move
	collision = Entity_move(ent, &ent->rect.y, &ent->velocity_y, y_distance, world);

	// if falling and collision happened, set grounded, else set non-grounded
	if (y_distance > 0.0f && collision)
		ent->grounded = TRUE;
	else
		ent->grounded = FALSE;
}
