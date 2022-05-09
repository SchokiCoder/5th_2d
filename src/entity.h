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

#ifndef ENTITY_H
#define ENTITY_H

#include <SG_entity.h>
#include "block.h"

static const float ENTITY_VELOCITY_THRESHOLD = 0.01f;
static const float ENTITY_GRAVITY = BLOCK_SIZE * 18;

typedef enum Entity
{
	E_NONE,
	E_PLAYER,

	E_LAST = E_PLAYER,
} Entity ;

static const SG_EntityData DATA_ENTITIES[] = {
	{
	.name = "none",
	.width = 0,
	.height = 0,
	.acceleration = 0,
	.max_velocity = 0,
	.decceleration = 0,
	.jump_velocity = 0,
	},

	{
	.name = "player",
	.width = (BLOCK_SIZE * 2) - 6,
	.height = (BLOCK_SIZE * 3) - 8,
	.acceleration = BLOCK_SIZE * 12,
	.max_velocity = BLOCK_SIZE * 6,
	.decceleration = BLOCK_SIZE * 6,
	.jump_velocity = BLOCK_SIZE * 12,
	},
};

void Entity_move_x( SG_Entity *ent, float x_distance, SG_World *world );

void Entity_move_y( SG_Entity *ent, float y_distance, SG_World *world );

#endif // ENTITY_H
