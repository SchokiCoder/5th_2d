/*
 * 2d_platformer
 * Copyright (C) 2022  Andy Frank Schoknecht
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not see
 * <https://www.gnu.org/licenses/old-licenses/gpl-2.0.html>.
 */

#ifndef BLOCK_H
#define BLOCK_H

#include <stdint.h>

static const int_fast32_t BLOCK_SIZE = 32;

typedef enum Block {
	B_NONE,
	B_DIRT,
	B_STONE,

	B_FIRST = B_NONE,
	B_LAST = B_STONE,
} Block;

#endif				/* BLOCK_H */
