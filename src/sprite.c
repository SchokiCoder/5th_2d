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
#include "log.h"
#include "sprite.h"

void sprite_create_texture( Sprite *sprite, SDL_Renderer *renderer, const char *source )
{
	char msg[LOG_MAX_LEN];

	// create texture
	sprite->texture = SDL_CreateTextureFromSurface(renderer, sprite->surface);

	// check
	if (sprite->texture == NULL)
	{
		sprintf(msg, "Sprite \"%s\" could not be initialized.", source);
		log_err(msg);
		sprite->invalid = true;
	}
}

Sprite sprite_from_file( SDL_Renderer *renderer, const char *filepath )
{
	Sprite result = {.invalid = false};

	// load image, create texture
	result.surface = IMG_Load(filepath);
	sprite_create_texture(&result, renderer, filepath);

	return result;
}

Sprite sprite_from_text( SDL_Renderer *renderer, const char *text, TTF_Font *font, SDL_Color color )
{
	Sprite result = {.invalid = false};

	// create text, create texture
	result.surface = TTF_RenderText_Solid(font, text, color);
	sprite_create_texture(&result, renderer, text);

	return result;
}

void clear_sprite( Sprite *sprite )
{
	//reset values
	sprite->invalid = false;

	// if surface and texture exist, clear them
	if (sprite->surface != NULL)
	{
		SDL_FreeSurface(sprite->surface);
		sprite->surface = NULL;
	}

	if (sprite->texture != NULL)
	{
		SDL_DestroyTexture(sprite->texture);
		sprite->texture = NULL;
	}
}
