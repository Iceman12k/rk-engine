/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
// g_local.h -- local definitions for game module

// define GAME_INCLUDE so that game.h does not define the
// short, server-visible gclient_t and edict_t structures,
// because we define the full size ones in this file
#define GAME_INCLUDE
#define USE_PROTOCOL_EXTENSIONS 1

#include "shared/shared.h"
#include "../q_shared.h"

#include "shared/list.h"
#include "shared/game.h"

#define EDICT_NUM(n) ((edict_t *)((byte *)g_edicts + sizeof(g_edicts[0])*(n)))
#define NUM_FOR_EDICT(e) ((int)(((byte *)(e) - (byte *)g_edicts) / sizeof(g_edicts[0])))

// features this game supports
#define G_FEATURES  (GMF_PROPERINUSE|GMF_WANT_ALL_DISCONNECTS|GMF_ENHANCED_SAVEGAMES|GMF_VARIABLE_FPS|GMF_PROTOCOL_EXTENSIONS)

// the "gameversion" client command will print this plus compile date
#define GAMEVERSION "zynfyr-q2"

// protocol bytes that can be directly added to messages
#define svc_muzzleflash     1
#define svc_muzzleflash2    2
#define svc_temp_entity     3
#define svc_layout          4
#define svc_inventory       5
#define svc_stufftext       11
#define svc_configstring	13

#define TAG_GAME    765     // clear when unloading the dll
#define TAG_LEVEL   766     // clear when loading a new level

extern const char *downloadlist[];
extern const int downloadlist_size;

//
// fields are needed for spawning from the entity string
// and saving / loading games
//
typedef enum {
	F_BAD,
	F_BYTE,
	F_SHORT,
	F_INT,
	F_BOOL,
	F_FLOAT,
	F_LSTRING,          // string on disk, pointer in memory, TAG_LEVEL
	F_GSTRING,          // string on disk, pointer in memory, TAG_GAME
	F_ZSTRING,          // string on disk, string in memory
	F_VECTOR,
	F_ANGLEHACK,
	F_EDICT,            // index on disk, pointer in memory
	F_ITEM,             // index on disk, pointer in memory
	F_CLIENT,           // index on disk, pointer in memory
	F_FUNCTION,
	F_POINTER,
	F_IGNORE
} fieldtype_t;

typedef gclient_t gclient_s;
typedef edict_t edict_s;

#define DEAD_NO					0
#define DEAD_DEAD				1

#define HUD_MAX_SIZE			4096

#define FOFS(x) q_offsetof(edict_t, x)
#define STOFS(x) q_offsetof(spawn_temp_t, x)
#define LLOFS(x) q_offsetof(level_locals_t, x)
#define GLOFS(x) q_offsetof(game_locals_t, x)
#define CLOFS(x) q_offsetof(gclient_t, x)

#define random()    frand()
#define crandom()   crand()

//
// this structure is left intact through an entire game
// it should be initialized at dll load time, and read/written to
// the server.ssv file for savegames
//
typedef struct {
	char        helpmessage1[512];
	char        helpmessage2[512];
	int         helpchanged;    // flash F1 icon if non 0, play sound
								// and increment only if 1, 2, or 3

	gclient_t   *clients;       // [maxclients]

	// can't store spawnpoint in level, because
	// it would get overwritten by the savegame restore
	char        spawnpoint[512];    // needed for coop respawns

	// store latched cvars here that we want to get at often
	int         maxclients;
	int         maxentities;

	// cross level triggers
	int         serverflags;
	int			serverfeatures;

	// variable fps
	int			framerate;
	float		frametime;
	int			framediv;

	cs_remap_t  csr;

} game_locals_t;


//
// this structure is cleared as each map is entered
// it is read/written to the level.sav file for savegames
//
typedef struct {
	edict_t		*current_entity;

	int         framenum;
	float       time;

	char        level_name[MAX_QPATH];  // the descriptive name (Outer Base, etc)
	char        mapname[MAX_QPATH];     // the server name (base1, etc)
	char        nextmap[MAX_QPATH];     // go here when fraglimit is hit

} level_locals_t;

// spawn_temp_t is only used to hold entity field values that
// can be set from the editor, but aren't actualy present
// in edict_t during gameplay
typedef struct {
	// world vars
	char        *sky;
	float       skyrotate;
	vec3_t      skyaxis;
	char        *nextmap;
	char        *musictrack;

	int         lip;
	int         distance;
	int         height;
	char        *noise;
	float       pausetime;
	char        *item;
	char        *gravity;

	float       minyaw;
	float       maxyaw;
	float       minpitch;
	float       maxpitch;
} spawn_temp_t;

typedef struct {
	char        userinfo[MAX_INFO_STRING];
	char        netname[16];
	int         fov;

	bool        connected;  // a loadgame will leave valid entities that
							// just don't have a connection yet

	bool        spectator;      // client is a spectator
} client_persistant_t;

// this structure is cleared on each PutClientInServer(),
// except for 'client->pers'
struct gclient_s {
	// known to server
	player_state_t  ps;             // communicated by server to clients
	int             ping;

	// private to game
	client_persistant_t pers;
	pmove_state_t       old_pmove;  // for detecting out-of-pmove changes
	vec3_t				v_angle;    // aiming direction
	vec3_t				cmd_angles;
	int					cmd_buttons;
	int					cmd_lastbuttons;

	float				time; // the "time" of the client, incremented by ucmd->msec
	int					download_cooldown; // when should we poke the client for the next asset download?
	int					download_progress;

#define FLOOD_MSGS  10
	float       flood_locktill;             // locked from talking
	float       flood_when[FLOOD_MSGS];     // when messages were said
	int         flood_whenhead;             // head pointer for when said

	qboolean	hotbar_open;
	int			hotbar_selected;
	float		hotbar_raisetime;
	float		hotbar_animtime;
	int			hotbar_wanted;

	qboolean	inv_open;
	float		inv_angle;
	
	int			passive_flags;

	//
	vec3_t		kick_angles;

	// our own version of level.framenum and level.time
	int 		levelframenum;
	float		leveltime;

};

struct edict_s {
    entity_state_t  s;
    struct gclient_s    *client;    // NULL if not a player
                                    // the server expects the first part
                                    // of gclient_s to be a player_state_t
                                    // but the rest of it is opaque

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

    //================================

#if USE_PROTOCOL_EXTENSIONS
    entity_state_extension_t    x;
#endif

	// DO NOT MODIFY ANYTHING ABOVE THIS, THE SERVER
	// EXPECTS THE FIELDS IN THAT ORDER!

	//================================
	char        *message;
	char        *classname;
	int         spawnflags;

	int         movetype;
	int         flags;

	char        *model;
	float       freetime;           // sv.time when the object was freed

	float       angle;          // set in qe3, -1 = up, -2 = down
	char        *target;
	char        *targetname;
	char        *killtarget;
	char        *team;
	char        *pathtarget;
	char        *deathtarget;
	char        *combattarget;
	edict_t     *target_ent;

	float       speed, accel, decel;
	vec3_t      movedir;
	vec3_t      pos1, pos2;

	vec3_t      velocity;
	vec3_t      avelocity;

	float       gravity;        // per entity gravity multiplier (1.0 is normal)
							// use for lowgrav artifact, flares

	int         viewheight;     // height above origin where eyesight is determined

	int         watertype;
	int         waterlevel;

	int         light_level;

	int			health;
	int			max_health;
	int			deadflag;

	edict_t     *chain;
	edict_t     *enemy;
	edict_t     *oldenemy;
	edict_t     *activator;
	edict_t     *groundentity;
	int         groundentity_linkcount;

	float       volume;
	float       attenuation;

	int         style;          // also used as areaportal number

	float		random;

	// q2 stuff
	int         nextthink;
    void        (*think)(edict_t *self);
	void        (*touch)(edict_t *self, edict_t *other, cplane_t *plane, csurface_t *surf);

	// monster stuff
	float		mass;

	// custom
	int			(*predraw)(edict_t *v, edict_t *e, entity_state_t *s, entity_state_extension_t *x);
	int			(*physics)(edict_t *e);
};


extern game_locals_t   game;
extern  level_locals_t  level;
extern  game_import_t   gi;
extern  game_import_ex_t gix;
extern  game_export_t   globals;
extern game_export_ex_t globalsx;
extern spawn_temp_t    st;

extern  edict_t  *g_edicts;

extern  cvar_t  *filterban;
extern  cvar_t  *maxclients;
extern  cvar_t  *maxspectators;
extern  cvar_t  *maxentities;

extern  cvar_t  *password;
extern  cvar_t  *spectator_password;
extern  cvar_t  *needpass;
extern  cvar_t  *dedicated;

extern  cvar_t  *sv_gravity;
extern  cvar_t  *sv_maxvelocity;

extern  cvar_t  *flood_msgs;
extern  cvar_t  *flood_persecond;
extern  cvar_t  *flood_waitdelay;

// variable server FPS
#ifndef NO_FPS
#define HZ              game.framerate
#define FRAMETIME       game.frametime
#define FRAMEDIV        game.framediv
#define FRAMESYNC       !(level.framenum % game.framediv)
#else
#define HZ              BASE_FRAMERATE
#define FRAMETIME       BASE_FRAMETIME_1000
#define FRAMEDIV        1
#define FRAMESYNC       1
#endif

#define KEYFRAME(x)   (level.framenum + (x) - (level.framenum % FRAMEDIV))

#define NEXT_FRAME(ent, func) \
    ((ent)->think = (func), (ent)->nextthink = level.framenum + 1)

#define NEXT_KEYFRAME(ent, func) \
    ((ent)->think = (func), (ent)->nextthink = KEYFRAME(FRAMEDIV))
//

//
// g_main.c
//
void* WriteData(const void *data, size_t len);
void ClientEndServerFrames(void);

//
// g_utils.c 
//
#define clamp(a,b,c) (a = (a <= c) ? ((a >= b) ? (a) : (b)) : (c))
#define bound(a,b,c) ((a) >= (c) ? (a) : (b) < (a) ? (a) : (b) > (c) ? (c) : (b))
float vectoyaw(vec3_t vec);
void MatrixMultiply(float in1[3][3], float in2[3][3], float out[3][3]);
void RotatePointAroundVector(vec3_t dst, const vec3_t dir, const vec3_t point, float degrees);
void vectoangles2(vec3_t angles, const vec3_t forward, const vec3_t up, qboolean flippitch);
void vectoangles(vec3_t value1, vec3_t angles);
int anglediff(int x, int y);


//
// g_svcmds.c
//
void ServerCommand(void);
bool SV_FilterPacket(char *from);

//
// g_svmain.c
//
void ShutdownGame(void);
void InitGame(void);
void WriteGame(const char *filename, qboolean autosave);
void ReadGame(const char *filename);
void WriteLevel(const char *filename);
void ReadLevel(const char *filename);
void G_RunFrame(void);

//
// g_spawn.c
//
void G_InitEdict(edict_t *e);
edict_t *G_Spawn(void);
void G_FreeEdict(edict_t *ed);
void SpawnEntities(const char *mapname, const char *entities, const char *spawnpoint);
void ED_ParseEdict(const char **data, edict_t *ent);
void ED_CallSpawn(edict_t *ent);

//
// p_base.c
//
void ClientThink(edict_t *ent, usercmd_t *ucmd);
void ClientBeginServerFrame(edict_t *ent);
void ClientEndServerFrame(edict_t *ent);
qboolean ClientConnect(edict_t *ent, char *userinfo);
void ClientUserinfoChanged(edict_t *ent, char *userinfo);
void ClientDisconnect(edict_t *ent);
void ClientBegin(edict_t *ent);

//
// p_cmd.c
//
void ClientCommand(edict_t *ent);

//
// g_phys.c 
//
int SV_FlyMove(edict_t *ent, float time, int mask);
trace_t SV_PushEntity(edict_t *ent, vec3_t push);
void SV_AddGravity(edict_t *ent);
void SV_Friction(edict_t *ent);