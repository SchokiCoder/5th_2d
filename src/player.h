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

#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>
#include "block.h"

// movement values (per second)
static const float PLAYER_ACCELERATION = (float) BLOCK_SIZE * 2.0;
static const float PLAYER_MAX_VELOCITY = (float) BLOCK_SIZE * 6.0;

typedef struct Rect
{
	float x, y, w, h;
} Rect ;

typedef struct Player
{
    Rect rect;
    float velocity_x, velocity_y;
} Player ;

void move_player_x( Player *player, float x_distance, World *world );
void move_player_y( Player *player, float y_distance, World *world );

#endif /* PLAYER_H */
