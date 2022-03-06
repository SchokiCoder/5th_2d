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
void move_player( Player *player, float *player_pos, float *player_velocity, float distance, World *world )
{
	SDL_Rect player_hitbox;
	SDL_Rect block_hitbox;
	SDL_Rect dummy;

	// set position
	*player_pos += distance;

	// collision check for nearby blocks
	player_hitbox.x = (int32_t) player->rect.x;
	player_hitbox.y = (int32_t) player->rect.y;
	player_hitbox.w = (int32_t) player->rect.w;
	player_hitbox.h = (int32_t) player->rect.h;

	for (
		int32_t x = player->rect.x / BLOCK_SIZE;
		x < (player->rect.x + player->rect.w) / BLOCK_SIZE;
		x++)
	{
		for (
			int32_t y = player->rect.y / BLOCK_SIZE;
			y < (player->rect.y + player->rect.h) / BLOCK_SIZE;
			y++)
		{
			// if block not within bounds
			if (x < 0 || x > WORLD_MAX_WIDTH - 1 ||
				y < 0 || y > WORLD_MAX_HEIGHT - 1)
				continue;

			// if non-solid block here, skip
			if (world->blocks[x][y] == B_NONE)
				continue;

			block_hitbox.x = x * BLOCK_SIZE;
			block_hitbox.y = y * BLOCK_SIZE;
			block_hitbox.w = BLOCK_SIZE;
			block_hitbox.h = BLOCK_SIZE;

			// if collision
			if (SDL_IntersectRect(&player_hitbox, &block_hitbox, &dummy)) {

				// reset pos, kill velocity
				*player_pos -= distance;
				*player_velocity = 0.0f;
			}
		}
	}
}

void move_player_x( Player *player, float x_distance, World *world )
{
	move_player(player, &player->rect.x, &player->velocity_x, x_distance, world);
}

void move_player_y( Player *player, float y_distance, World *world )
{
	move_player(player, &player->rect.y, &player->velocity_y, y_distance, world);
}
