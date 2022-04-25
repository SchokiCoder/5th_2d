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

#include <SM_log.h>
#include <SM_dict.h>
#include "path.h"
#include "config.h"

Config Config_new( void )
{
	Config cfg = {
		.invalid = false,
		.gfx_window_x = CFG_STD_GFX_WINDOW_X,
		.gfx_window_y = CFG_STD_GFX_WINDOW_Y,
		.gfx_window_w = CFG_STD_GFX_WINDOW_W,
		.gfx_window_h = CFG_STD_GFX_WINDOW_H,
	};

	return cfg;
}

void Config_load( Config *cfg )
{
	SM_String filepath = SM_String_new(16);
	SM_String msg = SM_String_new(1);

	// get path
	if (get_config_path(&filepath) != 0)
	{
		SM_String_clear(&filepath);
		cfg->invalid = true;
		return;
	}

	// read file
	SM_Dict dict = SM_Dict_from_file(filepath.str);

	if (!dict.valid)
	{
		SM_String_clear(&filepath);
		cfg->invalid = true;
		SM_log_warn("Config could not be loaded.");
		return;
	}

	// convert dict into config
    for (size_t i = 0; i < dict.len; i++)
    {
		// window pos, size
		if (SM_strequal(dict.data[i].key.str, CFG_SETTING_GFX_WINDOW_X))
			cfg->gfx_window_x = strtol(dict.data[i].value.str, NULL, 10);

		else if(SM_strequal(dict.data[i].key.str, CFG_SETTING_GFX_WINDOW_Y))
			cfg->gfx_window_y = strtol(dict.data[i].value.str, NULL, 10);

		else if(SM_strequal(dict.data[i].key.str, CFG_SETTING_GFX_WINDOW_W))
			cfg->gfx_window_w = strtol(dict.data[i].value.str, NULL, 10);

		else if(SM_strequal(dict.data[i].key.str, CFG_SETTING_GFX_WINDOW_H))
			cfg->gfx_window_h = strtol(dict.data[i].value.str, NULL, 10);

		// unknown option
		else
		{
			SM_String_empty(&msg);
			SM_String_append_cstr(&msg, "Unknown config setting \"");
			SM_String_append(&msg, &dict.data[i].key);
			SM_String_append_cstr(&msg, "\".");
			SM_log_warn(msg.str);
		}
	}

	SM_String_clear(&filepath);
	SM_String_clear(&msg);
	SM_Dict_clear(&dict);
}

void Config_save( Config *cfg )
{
	SM_String filepath = SM_String_new(16);

	/* get path */
	if (get_config_path(&filepath) != 0)
	{
		SM_String_clear(&filepath);
		cfg->invalid = true;
		return;
	}

	// convert config into dict
	SM_Dict dict = SM_Dict_new(1);
	char temp[10];

	sprintf(temp, "%i", cfg->gfx_window_x);
	SM_Dict_add(&dict, CFG_SETTING_GFX_WINDOW_X, temp);
	sprintf(temp, "%i", cfg->gfx_window_y);
	SM_Dict_add(&dict, CFG_SETTING_GFX_WINDOW_Y, temp);
	sprintf(temp, "%i", cfg->gfx_window_w);
	SM_Dict_add(&dict, CFG_SETTING_GFX_WINDOW_W, temp);
	sprintf(temp, "%i", cfg->gfx_window_h);
	SM_Dict_add(&dict, CFG_SETTING_GFX_WINDOW_H, temp);

	// save
	if (!SM_Dict_write(&dict, filepath.str))
		cfg->invalid = true;

	// clear
	SM_String_clear(&filepath);
	SM_Dict_clear(&dict);
}
