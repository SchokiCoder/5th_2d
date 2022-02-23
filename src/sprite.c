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

#include <SDL_image.h>
#include <SDL_ttf.h>
#include "sprite.h"

Sprite Sprite_from_file( SDL_Renderer *renderer, const char *filepath ) {
	Sprite result;

	// load image
	result.surface = IMG_Load(filepath);

	// create texture
	result.texture = SDL_CreateTextureFromSurface(renderer, result.surface);

	// return
	return result;
}

Sprite Sprite_from_text( SDL_Renderer *renderer, const char *text, TTF_Font *font, SDL_Color color ) {
	Sprite result;

	// create text
	result.surface = TTF_RenderText_Solid(font, text, color);

	// create texture
	result.texture = SDL_CreateTextureFromSurface(renderer, result.surface);

	// return
	return result;
}

void Sprite_clear( Sprite *self ) {
	// if surface and texture exist, clear them
	if (self->surface != NULL)
	{
		SDL_FreeSurface(self->surface);
		self->surface = NULL;
	}

	if (self->texture != NULL)
	{
		SDL_DestroyTexture(self->texture);
		self->texture = NULL;
	}
}
