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

#ifndef BLOCK_H
#define BLOCK_H

#include <stdint.h>

static const uint32_t BLOCK_SIZE = 32;

typedef enum Block
{
	B_NONE,
	B_DIRT,
	B_STONE
} Block ;
#define NUM_BLOCK_TYPES 3

#endif /* BLOCK_H */
