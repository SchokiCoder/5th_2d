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

#ifndef TYPES_H
#define TYPES_H

typedef struct FPoint
{
	float x;
	float y;
} FPoint ;

typedef struct IPoint
{
	int x;
	int y;
} IPoint ;

typedef struct FRect
{
	float x;
	float y;
	float w;
	float h;
} FRect ;

#endif // TYPES_H
