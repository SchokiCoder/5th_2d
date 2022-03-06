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

#ifndef SPRITE_H
#define SPRITE_H

#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_ttf.h>

typedef struct Sprite
{
	bool invalid;
	SDL_Surface *surface;
	SDL_Texture *texture;
} Sprite ;

Sprite sprite_from_file( SDL_Renderer*, const char* );

Sprite sprite_from_text( SDL_Renderer*, const char*, TTF_Font*, SDL_Color);

void sprite_create_texture( Sprite *sprite, SDL_Renderer *renderer, const char *source );

void clear_sprite( Sprite* );

#endif /* SPRITE_H */
