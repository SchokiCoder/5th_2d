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

#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>

static const char CFG_SETTING_GFX_WINDOW_X[] =			"gfx_window_x";
static const char CFG_SETTING_GFX_WINDOW_Y[] =			"gfx_window_y";
static const char CFG_SETTING_GFX_WINDOW_W[] =			"gfx_window_w";
static const char CFG_SETTING_GFX_WINDOW_H[] =			"gfx_window_h";
static const char CFG_SETTING_GFX_WINDOW_FULLSCREEN[] =	"gfx_window_fullscreen";

static const uint32_t CFG_STD_GFX_WINDOW_X =		SDL_WINDOWPOS_CENTERED;
static const uint32_t CFG_STD_GFX_WINDOW_Y =		SDL_WINDOWPOS_CENTERED;
static const float CFG_STD_GFX_WINDOW_W =			640.0f;
static const float CFG_STD_GFX_WINDOW_H =			480.0f;
static const bool CFG_STD_GFX_WINDOW_FULLSCREEN =	false;

typedef struct Config
{
	bool invalid;
	int32_t gfx_window_x;
	int32_t gfx_window_y;
	int32_t gfx_window_w;
	int32_t gfx_window_h;
	bool gfx_window_fullscreen;
} Config ;

Config Config_new( void );

void Config_load( Config *cfg );

void Config_save( Config *cfg );

#endif // CONFIG_H
