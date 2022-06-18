/*
	2d_platformer
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

#ifndef GAME_H
#define GAME_H

#include <SG_types.h>
#include <SDL_render.h>
#include "entity.h"
#include "block.h"

typedef struct Config Config;

float now( void );

typedef struct Game
{
	char *world_name;
	SDL_Renderer *renderer;
	Config *cfg;

	SM_String msg;
	bool active;
	SGUI_Sprite spr_blocks[B_LAST + 1];
	SGUI_Sprite spr_walls[B_LAST + 1];
	SGUI_Sprite spr_ents[E_LAST + 1];
	SG_World world;
	SDL_Event event;
	const uint8_t *kbd;
    SG_IPoint wld_draw_pts[2];
    SDL_Rect camera;
} Game ;

void Game_setup( Game *game );

void Game_run( Game *game );

void Game_edit( Game *game, const size_t width, const size_t height );

void Game_clear( Game *game );

#endif
