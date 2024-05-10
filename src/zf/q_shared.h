

#include "pmove.h"

// player state
enum {
	AMMO_SHELLS,
	AMMO_BULLETS,
	AMMO_GRENADES,
	AMMO_ROCKETS,
	AMMO_CELLS,
	AMMO_PLASMA,
	AMMO_SLUGS,
	AMMO_MAX,
};

typedef struct {
	uint8_t		weapon;
	uint8_t		weapons[8];
	uint16_t	ammo[AMMO_MAX];

	uint16_t	attack_finished;
	uint16_t	busy_finished;
} pweapon_state_t;

typedef struct {
    pmove_state_t   pmove;      // for prediction

    // these fields do not need to be communicated bit-precise

    vec3_t      viewangles;     // for fixed views
    vec3_t      viewoffset;     // add to pmovestate->origin
    vec3_t      kick_angles;    // add to view direction to get render angles
                                // set by weapon kicks, pain effects, etc

    vec3_t      gunangles;
    vec3_t      gunoffset;
    int         gunindex;
    int         gunframe;

    float       blend[4];       // rgba full screen effect

    float       fov;            // horizontal field of view

    int         rdflags;        // refdef flags

    short       stats[MAX_STATS];       // fast status bar updates

	union {
		uint8_t dat[64];
		struct {
			pweapon_state_t pwep;
		};
	};
} player_state_t;
//

// entity state
enum {
	ENT_NORMAL,
	ENT_DETAIL,
	ENT_PLAYER,
};

typedef struct entity_state_s {
    int     number;         // edict index

    vec3_t  origin;
    vec3_t  angles;
    vec3_t  old_origin;     // for lerping
    int     modelindex;
    int     modelindex2, modelindex3, modelindex4;  // weapons, CTF flags, etc
    int     frame;
    int     skinnum;
    unsigned int        effects;        // PGM - we're filling it, so it needs to be unsigned
    int     renderfx;
    int     solid;          // for client side prediction, 8*(bits 0-4) is x/y radius
                            // 8*(bits 5-9) is z down distance, 8(bits10-15) is z up
                            // gi.linkentity sets this properly
    int     sound;          // for looping sounds, to guarantee shutoff
    int     event;          // impulse events -- muzzle flashes, footsteps, etc
                            // events only go out for a single frame, they
                            // are automatically cleared each frame
	union {
		uint8_t dat[64];
		struct {
			uint8_t type;
		};
	};
} entity_state_t;
//

// packed versions
typedef struct {
    uint16_t    number;
    int16_t     origin[3];
    int16_t     angles[3];
    int16_t     old_origin[3];
    uint16_t    modelindex;
    uint16_t    modelindex2;
    uint16_t    modelindex3;
    uint16_t    modelindex4;
    uint32_t    skinnum;
    uint32_t    effects;
    uint32_t    renderfx;
    uint32_t    solid;
    uint32_t    morefx;
    uint16_t    frame;
    uint16_t    sound;
    uint8_t     event;
    uint8_t     alpha;
    uint8_t     scale;
    uint8_t     loop_volume;
    uint8_t     loop_attenuation;
	union {
		uint8_t 	data[64];
		struct {
			uint8_t type;
		};
	};
} entity_packed_t;

typedef struct {
    pmove_state_t   pmove;
    int16_t         viewangles[3];
    int8_t          viewoffset[3];
    int8_t          kick_angles[3];
    int8_t          gunangles[3];
    int8_t          gunoffset[3];
    uint16_t        gunindex;
    uint8_t         gunframe;
    uint8_t         blend[4];
    uint8_t         fov;
    uint8_t         rdflags;
    int16_t         stats[MAX_STATS];
	union {
		uint8_t 	data[64];
		struct {
			pweapon_state_t pwep;
		};
	};
} player_packed_t;

typedef enum {
    MSG_PS_IGNORE_GUNINDEX      = BIT(0),   // ignore gunindex
    MSG_PS_IGNORE_GUNFRAMES     = BIT(1),   // ignore gunframe/gunoffset/gunangles
    MSG_PS_IGNORE_BLEND         = BIT(2),   // ignore blend
    MSG_PS_IGNORE_VIEWANGLES    = BIT(3),   // ignore viewangles
    MSG_PS_IGNORE_DELTAANGLES   = BIT(4),   // ignore delta_angles
    MSG_PS_IGNORE_PREDICTION    = BIT(5),   // mutually exclusive with IGNORE_VIEWANGLES
    MSG_PS_EXTENSIONS           = BIT(6),   // enable protocol extensions
    MSG_PS_FORCE                = BIT(7),   // send even if unchanged (MVD stream only)
    MSG_PS_REMOVE               = BIT(8),   // player is removed (MVD stream only)
} msgPsFlags_t;

typedef enum {
    MSG_ES_FORCE        = BIT(0),   // send even if unchanged
    MSG_ES_NEWENTITY    = BIT(1),   // send old_origin
    MSG_ES_FIRSTPERSON  = BIT(2),   // ignore origin/angles
    MSG_ES_LONGSOLID    = BIT(3),   // higher precision bbox encoding
    MSG_ES_UMASK        = BIT(4),   // client has 16-bit mask MSB fix
    MSG_ES_BEAMORIGIN   = BIT(5),   // client has RF_BEAM old_origin fix
    MSG_ES_SHORTANGLES  = BIT(6),   // higher precision angles encoding
    MSG_ES_EXTENSIONS   = BIT(7),   // enable protocol extensions
    MSG_ES_REMOVE       = BIT(8),   // entity is removed (MVD stream only)
} msgEsFlags_t;
//

void Pmove(pmove_t *pmove, pweapon_state_t *pweapon, const pmoveParams_t *params);

#define ANGLE2BYTE(x)   ((int)((x)*256.0f/360)&255)
#define BYTE2ANGLE(x)   ((x)*(360.0f/256))

#define MSG_WriteByte(a) gi.WriteByte(a)
#define MSG_WriteChar(a) gi.WriteChar(a)
#define MSG_WriteShort(a) gi.WriteShort(a)
#define MSG_WriteLong(a) gi.WriteLong(a)
#define MSG_WriteAngle(a) gi.WriteAngle(a)
#define MSG_WriteFloat(a) gi.WriteFloat(a)

#define MSG_ReadByte(a) gi.ReadByte(a)
#define MSG_ReadChar(a) gi.ReadChar(a)
#define MSG_ReadShort(a) gi.ReadShort(a)
#define MSG_ReadLong(a) gi.ReadLong(a)
#define MSG_ReadFloat(a) gi.ReadFloat(a)
