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

typedef struct FPoint
{
	float x, y;
} FPoint ;

bool point_within_box( FPoint *pt, FRect *box )
{
    if (pt->x > box->x && pt->x < (box->x + box->w) &&
    	pt->y > box->y && pt->y < (box->y + box->h)) {
		return true;
	}

	return false;
}

bool box_within_box( FRect *box_1, FRect *box_2 )
{
    //SDL_Point pt;
    FPoint pt;

    // is box 1 within box 2
    pt.x = box_1->x;
    pt.y = box_1->y;

    if (point_within_box(&pt, box_2)) {
    	return true;
	}

	pt.x = box_1->x + box_1->w;
    pt.y = box_1->y;

    if (point_within_box(&pt, box_2)) {
    	return true;
	}

	pt.x = box_1->x;
    pt.y = box_1->y + box_1->h;

    if (point_within_box(&pt, box_2)) {
    	return true;
	}

	pt.x = box_1->x + box_1->w;
    pt.y = box_1->y + box_1->h;

    if (point_within_box(&pt, box_2)) {
    	return true;
	}

	// is box 2 within box 1
	pt.x = box_2->x;
    pt.y = box_2->y;

    if (point_within_box(&pt, box_1)) {
    	return true;
	}

	pt.x = box_2->x + box_1->w;
    pt.y = box_2->y;

    if (point_within_box(&pt, box_1)) {
    	return true;
	}

	pt.x = box_2->x;
    pt.y = box_2->y + box_1->h;

    if (point_within_box(&pt, box_1)) {
    	return true;
	}

	pt.x = box_2->x + box_1->w;
    pt.y = box_2->y + box_1->h;

    if (point_within_box(&pt, box_1)) {
    	return true;
	}

    return false;
}

/*
	player_pos: 		player.rect.x or y
	player_velocity:	player.velocity_x or y
*/
bool move_player( Player *player, float *player_pos, float *player_velocity, float distance, World *world )
{
	bool collision = false;
	int_fast32_t x1, y1, x2, y2;
	FRect block_hitbox;

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
			if (world->blocks[x][y] == B_NONE)
				continue;

			block_hitbox.x = x * BLOCK_SIZE;
			block_hitbox.y = y * BLOCK_SIZE;

			// if collision
			if (box_within_box(&player->rect, &block_hitbox))
			{
				// flag, reset pos, kill velocity
				collision = true;
				*player_pos -= distance;
				*player_velocity = 0.0f;
			}
		}
	}

	return collision;
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
