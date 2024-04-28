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
// g_local.h -- local definitions for game module

#include "shared/shared.h"
#include "../q_list.h"

#define CGAME_API_VERSION 1

#define TAG_GAME    765     // clear when unloading the dll
#define TAG_LEVEL   766     // clear when loading a new level

// define GAME_INCLUDE so that game.h does not define the
// short, server-visible gclient_t and edict_t structures,
// because we define the full size ones in this file
#define GAME_INCLUDE

// the "gameversion" client command will print this plus compile date
#define GAMEVERSION "ZynFyr"

// edict->flags
#define FL_FLY                  0x00000001
#define FL_SWIM                 0x00000002  // implied immunity to drowining
#define FL_IMMUNE_LASER         0x00000004
#define FL_INWATER              0x00000008
#define FL_GODMODE              0x00000010
#define FL_NOTARGET             0x00000020
#define FL_IMMUNE_SLIME         0x00000040
#define FL_IMMUNE_LAVA          0x00000080
#define FL_PARTIALGROUND        0x00000100  // not all corners are valid
#define FL_WATERJUMP            0x00000200  // player jumping out of water
#define FL_TEAMSLAVE            0x00000400  // not the first on the team
#define FL_NO_KNOCKBACK         0x00000800
#define FL_POWER_ARMOR          0x00001000  // power armor (if any) is active
#define FL_NOCLIP_PROJECTILE    0x00002000  // projectile hack
#define FL_MEGAHEALTH           0x00004000  // for megahealth kills tracking
#define FL_ACCELERATE           0x20000000  // accelerative movement
#define FL_HIDDEN               0x40000000  // used for banned items
#define FL_RESPAWN              0x80000000  // used for item respawning

#define MAX_ENT_CLUSTERS    16

typedef struct edict_s edict_t;

typedef enum {
	SOLID_NOT,          // no interaction with other objects
	SOLID_TRIGGER,      // only touch when inside, after moving
	SOLID_BBOX,         // touch on edge
	SOLID_BSP           // bsp clip, touch on edge
} solid_t;

struct edict_s {
	entity_state_t  s;

	qboolean    inuse;
	int         linkcount;

	// FIXME: move these fields to a server private sv_entity_t
	list_t      area;               // linked to a division node or leaf

	int         num_clusters;       // if -1, use headnode instead
	int         clusternums[MAX_ENT_CLUSTERS];
	int         headnode;           // unused if num_clusters != -1
	int         areanum, areanum2;

	//================================

	int         svflags;
	vec3_t      mins, maxs;
	vec3_t      absmin, absmax, size;
	solid_t     solid;
	int         clipmask;
	edict_t     *owner;


	// DO NOT MODIFY ANYTHING ABOVE THIS, THE SERVER
	// EXPECTS THE FIELDS IN THAT ORDER!
};

//
// functions provided by the main engine
//
typedef struct {
	// special messages
	void(*q_printf(1, 2) dprintf)(const char *fmt, ...);
	void(*q_noreturn q_printf(1, 2) error)(const char *fmt, ...);
} cgame_import_t;

//
// functions exported by the game subsystem
//
typedef struct {
	int         apiversion;

	// the init function will only be called when a game starts,
	// not each time a level is loaded.  Persistant data for clients
	// and the server can be allocated in init
	void(*Init)(void);
	void(*Shutdown)(void);
	void(*SendUserInput)(usercmd_t *oldcmd, usercmd_t *cmd);

} cgame_export_t;

extern  cgame_import_t   gi;
extern  cgame_export_t   globals;


void    Com_LPrintf(print_type_t type, const char *fmt, ...);
#define Com_Printf(...) Com_LPrintf(PRINT_ALL, __VA_ARGS__)

//cg_input.c
void CG_SendUserInput(usercmd_t *oldcmd, usercmd_t *cmd);

