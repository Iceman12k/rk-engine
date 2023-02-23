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

#ifndef GAME_HARD_LINKED
// this is only here so the functions in q_shared.c can link
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
#endif