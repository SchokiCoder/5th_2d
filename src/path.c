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

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <SM_log.h>
#include "app.h"
#include "path.h"

int32_t get_base_path( SM_String *out )
{
	int32_t rc;

	// get path
	SM_String_append_cstr(out, getenv("HOME"));
	SM_String_append_cstr(out, SLASH);
	SM_String_append_cstr(out, ".");
	SM_String_append_cstr(out, APP_NAME);
	SM_String_append_cstr(out, SLASH);

	/* in case, create dir */
	errno = 0;

	#ifdef _WIN32
		rc = mkdir(out->str);
	#else
		rc = mkdir(out->str, S_IRWXU);
	#endif

	if (rc == -1)
	{
		if (errno != EEXIST)
		{
			SM_log_err("Could not get userfiles path.");
			return 1;
		}
	}

	return 0;
}

int32_t get_world_path( SM_String *out )
{
	int32_t rc;

	/* get base path */
	rc = get_base_path(out);

	if (rc != 0)
		return rc;

	/* get path */
	SM_String_append_cstr(out, PATH_WORLDS);
	SM_String_append_cstr(out, SLASH);

	/* in case, create dir */
	errno = 0;

	#ifdef _WIN32
		rc = mkdir(out->str);
	#else
		rc = mkdir(out->str, S_IRWXU);
	#endif

	if (rc == -1)
	{
		if (errno != EEXIST)
		{
			SM_log_err("Could not create worlds directory.");
			return 1;
		}
	}

	return 0;
}

int32_t get_config_path( SM_String *out )
{
	int32_t rc;

	/* get base path */
	rc = get_base_path(out);

	if (rc != 0)
		return rc;

	/* get path */
	SM_String_append_cstr(out, PATH_CONFIG);

	return 0;
}
