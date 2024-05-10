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

#define CGAME_API_VERSION 1

#define TAG_GAME    765     // clear when unloading the dll
#define TAG_LEVEL   766     // clear when loading a new level

// define GAME_INCLUDE so that game.h does not define the
// short, server-visible gclient_t and edict_t structures,
// because we define the full size ones in this file
#define CGAME_INCLUDE
#define GAME_INCLUDE
#define USE_PROTOCOL_EXTENSIONS 1

#include "shared/shared.h"
#include "../q_shared.h"

#include "../q_list.h"
#include "../gameplay.h"
#include "../pmove.h"

#include "common/protocol.h"

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

// ui flags
#define UI_LEFT             BIT(0)
#define UI_RIGHT            BIT(1)
#define UI_CENTER           (UI_LEFT | UI_RIGHT)
#define UI_BOTTOM           BIT(2)
#define UI_TOP              BIT(3)
#define UI_MIDDLE           (UI_BOTTOM | UI_TOP)
#define UI_DROPSHADOW       BIT(4)
#define UI_ALTCOLOR         BIT(5)
#define UI_IGNORECOLOR      BIT(6)
#define UI_XORCOLOR         BIT(7)
#define UI_AUTOWRAP         BIT(8)
#define UI_MULTILINE        BIT(9)
#define UI_DRAWCURSOR       BIT(10)

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


#include "format/bsp.h"
#define MAX_MAP_AREA_BYTES      (MAX_MAP_AREAS / 8)
#define MAX_MAP_PORTAL_BYTES    128

typedef struct {
    bool            valid;

    int             number;
    int             delta;

    byte            areabits[MAX_MAP_AREA_BYTES];
    int             areabytes;

    player_state_t  ps;
    int             clientNum;

    int             numEntities;
    int             firstEntity;
} server_frame_t;

typedef struct {
	server_frame_t 	frame;
	server_frame_t 	oldframe;

	pmoveParams_t 	pmp;
	usercmd_t		cmd;
	usercmd_t		cmds[CMD_BACKUP];

	short			predicted_origins[CMD_BACKUP][3];
	vec3_t			localmove;

	int				servertime;
	int				serverdelta;
	int				time;
	float			lerpfrac;
} cgame_state_t;


//
// functions provided by the main engine
//
typedef struct {
	// special messages
	void		(*q_printf(1, 2) dprintf)(const char *fmt, ...);
	void		(*q_noreturn q_printf(1, 2) error)(const char *fmt, ...);
	void		*(*GetExtension)(const char *name);

	int		(*modelindex)(const char *name);

	int 	(*ReadChar)(void);
	int 	(*ReadByte)(void);
	int 	(*ReadShort)(void);
	int 	(*ReadLong)(void);
	float 	(*ReadFloat)(void);
	void 	(*ReadString)(char *dest, size_t size);
	void 	(*ReadPosition)(vec3_t pos);			// some fractional bits
	void 	(*ReadDir)(vec3_t pos);					// single byte encoded, very coarse

	trace_t (* q_gameabi trace)(const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int contentmask);
    int (*pointcontents)(const vec3_t point);
} cgame_import_t;

typedef struct {
	void 		(*R_DrawStretchPic)(int x, int y, int w, int h, const char *name);
	void		(*R_DrawString)(int x, int y, int flags, size_t maxChars, const char *string);
	void 		(*R_SetClipArea)(int x, int y, int w, int h);
	void 		(*R_ResetClipArea)(void);
} cgame_import_extensions_t;

//
// functions exported by the game subsystem
//
typedef struct {
	int         apiversion;

	void		(*Init)(void);
	void		(*Shutdown)(void);
	void		*(*GetExtension)(const char *name);
} cgame_export_t;

extern  cgame_import_t   gi;
extern  cgame_export_t   globals;
extern  cgame_import_extensions_t gx; 
extern  cgame_state_t    *cl;

#define clamp(a,b,c) (a = (a <= c) ? ((a >= b) ? (a) : (b)) : (c))
#define bound(a,b,c) ((a) >= (c) ? (a) : (b) < (a) ? (a) : (b) > (c) ? (c) : (b))

void    Com_LPrintf(print_type_t type, const char *fmt, ...);
#define Com_Printf(...) Com_LPrintf(PRINT_ALL, __VA_ARGS__)

// cg_networking.c
void CG_ReadDeltaEntity(entity_state_t *to, entity_state_extension_t *ext, int number, uint64_t bits, msgEsFlags_t flags);
void CG_RunPrediction(pmove_t *pm, int *o_current, int *o_ack, int *o_frame);

// cg_ui.c
void CG_UI_Render(vec2_t screensize);

