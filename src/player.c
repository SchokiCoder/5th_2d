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

#include "world.h"
#include "player.h"

/*
	player_pos: 		player.rect.x or y
	player_velocity:	player.velocity_x or y
*/
bool move_player( Player *player, float *player_pos, float *player_velocity, float distance, World *world )
{
	bool result = false;
	int32_t x1, y1, x2, y2;
	SDL_Rect player_hitbox;
	SDL_Rect block_hitbox;
	SDL_Rect dummy;

	// set position
	*player_pos += distance;

	// calculate which blocks to check
	x1 = player->rect.x / BLOCK_SIZE;
	y1 = player->rect.y / BLOCK_SIZE;
	x2 = (player->rect.x + player->rect.w) / BLOCK_SIZE + 1;
	y2 = (player->rect.y + player->rect.h) / BLOCK_SIZE + 1;

#ifdef _DEBUG
	// update check box
	player->box.x = x1 * BLOCK_SIZE;
	player->box.y = y1 * BLOCK_SIZE;
	player->box.w = (x2 * BLOCK_SIZE) - player->box.x;
	player->box.h = (y2 * BLOCK_SIZE) - player->box.y;
#endif

	// check if within bounds
	if (x1 < 0)
		x1 = 0;

	if (x1 > WORLD_MAX_WIDTH - 1)
		x1 = WORLD_MAX_WIDTH - 1;

	if (y1 < 0)
		y1 = 0;

	if (y1 > WORLD_MAX_HEIGHT - 1)
		y1 = WORLD_MAX_HEIGHT - 1;

	if (x2 < 0)
		x2 = 0;

	if (x2 > WORLD_MAX_WIDTH - 1)
		x2 = WORLD_MAX_WIDTH - 1;

	if (y2 < 0)
		y2 = 0;

	if (y2 > WORLD_MAX_HEIGHT - 1)
		y2 = WORLD_MAX_HEIGHT - 1;

	// collision check for nearby blocks
	player_hitbox.x = (int32_t) player->rect.x;
	player_hitbox.y = (int32_t) player->rect.y;
	player_hitbox.w = (int32_t) player->rect.w;
	player_hitbox.h = (int32_t) player->rect.h;

	block_hitbox.w = BLOCK_SIZE;
	block_hitbox.h = BLOCK_SIZE;

	for (; x1 < x2; x1++)
	{
		for (; y1 < y2; y1++)
		{
			// if non-solid block here, skip
			if (world->blocks[x1][y1] == B_NONE)
				continue;

			block_hitbox.x = x1 * BLOCK_SIZE;
			block_hitbox.y = y1 * BLOCK_SIZE;

			// if collision
			if (SDL_IntersectRect(&player_hitbox, &block_hitbox, &dummy))
			{
				// flag, reset pos, kill velocity
				result = true;
				*player_pos -= distance;
				*player_velocity = 0.0f;
			}
		}
	}

	return result;
}

void move_player_x( Player *player, float x_distance, World *world )
{
	move_player(player, &player->rect.x, &player->velocity_x, x_distance, world);
}

void move_player_y( Player *player, float y_distance, World *world )
{
	bool collision;

	// move
	collision = move_player(player, &player->rect.y, &player->velocity_y, y_distance, world);

	// if falling and collision happened, set grounded, else set non-grounded
	if (y_distance > 0.0f && collision)
		player->grounded = true;
	else
		player->grounded = false;
}
