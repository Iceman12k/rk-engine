/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "cg_local.h"
#include <errno.h>

cgame_import_t   gi;
cgame_export_t   globals;

void Com_LPrintf(print_type_t type, const char *fmt, ...)
{
	va_list     argptr;
	char        text[MAX_STRING_CHARS];

	if (type == PRINT_DEVELOPER) {
		return;
	}

	va_start(argptr, fmt);
	Q_vsnprintf(text, sizeof(text), fmt, argptr);
	va_end(argptr);

	gi.dprintf("%s", text);
}

void Com_Error(error_type_t type, const char *fmt, ...)
{
	va_list     argptr;
	char        text[MAX_STRING_CHARS];

	va_start(argptr, fmt);
	Q_vsnprintf(text, sizeof(text), fmt, argptr);
	va_end(argptr);

	gi.error("%s", text);
}


/*
============
InitGame

This will be called when the dll is first loaded, which
only happens when a new game is started or a save game
is loaded.
============
*/
static void G_Init(void)
{
	gi.dprintf("==== CGame Initialized ====\n");
}


static void G_Shutdown(void)
{
	gi.dprintf("==== ShutdownGame ====\n");

	//gi.FreeTags(TAG_LEVEL);
	//gi.FreeTags(TAG_GAME);
}


/*
=================
GetCGameAPI

Returns a pointer to the structure with all entry points
and global variables
=================
*/
q_exported cgame_export_t *GetCGameAPI(cgame_import_t *import)
{
	gi = *import;
	gi.dprintf("==== %s ====\n", __func__);

	globals.apiversion = CGAME_API_VERSION;
	globals.Init = G_Init;
	globals.Shutdown = G_Shutdown;
	globals.SendUserInput = CG_SendUserInput;

	return &globals;
}





