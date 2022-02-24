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

#include <SDL.h>
#include <stdint.h>
#include "block.h"

static const uint32_t PLAYER_WIDTH = BLOCK_SIZE * 2 - 6;
static const uint32_t PLAYER_HEIGHT = BLOCK_SIZE * 3 - 8;

enum EntityType {
	E_NONE,
	E_PLAYER
};

typedef struct Entity {
	enum EntityType type;
	SDL_Rect rect;
} Entity ;

#endif /* ENTITY_H */
