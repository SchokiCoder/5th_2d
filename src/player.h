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
/*
#include <stdint.h>
#include <stdbool.h>
#include "block.h"

// movement values
static const float PLAYER_VELOCITY_THRESHOLD = 0.01f;
static const float PLAYER_ACCELERATION = (float) BLOCK_SIZE * 12.0f;
static const float PLAYER_MAX_VELOCITY = (float) BLOCK_SIZE * 6.0f;
static const float PLAYER_WALKING_FRICTION = PLAYER_ACCELERATION / 2.0f;
static const float GRAVITY = (float) BLOCK_SIZE * 18.0f;
static const float PLAYER_JUMP_VELOCITY = (float) BLOCK_SIZE * 12.0f;

typedef struct Player
{
#ifdef _DEBUG
    SDL_Rect box;	// optical box in which collision checks with blocks happen
#endif

    FRect rect;		// actual hitbox
    float velocity_x, velocity_y;
    bool grounded;
} Player ;

void move_player_x( Player *player, float x_distance, World *world );

void move_player_y( Player *player, float y_distance, World *world );
*/
#endif /* PLAYER_H */
