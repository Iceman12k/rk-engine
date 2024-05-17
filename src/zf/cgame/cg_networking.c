

#include "cg_local.h"

inline vec_t MSG_ReadCoord(void)
{
	return SHORT2COORD(MSG_ReadShort());
}

inline void MSG_ReadPos(vec3_t v)
{
	v[0] = MSG_ReadCoord();
	v[1] = MSG_ReadCoord();
	v[2] = MSG_ReadCoord();
}

inline float MSG_ReadAngle16(void)
{
	return SHORT2ANGLE(MSG_ReadShort());
}

inline float MSG_ReadAngle(void)
{
	return BYTE2ANGLE(MSG_ReadShort());
}

void CG_ReadDeltaEntity(entity_state_t *to, entity_state_extension_t *ext, int number, uint64_t bits, msgEsFlags_t flags)
{
	Q_assert(to);
	Q_assert(number > 0 && number < MAX_EDICTS);

	to->number = number;
	to->event = 0;

	if (!bits) {
		return;
	}

	if (flags & MSG_ES_EXTENSIONS && bits & U_MODEL16) {
		if (bits & U_MODEL ) to->modelindex  = MSG_ReadShort();
		if (bits & U_MODEL2) to->modelindex2 = MSG_ReadShort();
		if (bits & U_MODEL3) to->modelindex3 = MSG_ReadShort();
		if (bits & U_MODEL4) to->modelindex4 = MSG_ReadShort();
	} else {
		if (bits & U_MODEL ) to->modelindex  = MSG_ReadByte();
		if (bits & U_MODEL2) to->modelindex2 = MSG_ReadByte();
		if (bits & U_MODEL3) to->modelindex3 = MSG_ReadByte();
		if (bits & U_MODEL4) to->modelindex4 = MSG_ReadByte();
	}

	if (bits & U_FRAME8)
		to->frame = MSG_ReadByte();
	if (bits & U_FRAME16)
		to->frame = MSG_ReadShort();

	if ((bits & U_SKIN32) == U_SKIN32)
		to->skinnum = MSG_ReadLong();
	else if (bits & U_SKIN8)
		to->skinnum = MSG_ReadByte();
	else if (bits & U_SKIN16)
		to->skinnum = MSG_ReadShort();

	if ((bits & U_EFFECTS32) == U_EFFECTS32)
		to->effects = MSG_ReadLong();
	else if (bits & U_EFFECTS8)
		to->effects = MSG_ReadByte();
	else if (bits & U_EFFECTS16)
		to->effects = MSG_ReadShort();

	if ((bits & U_RENDERFX32) == U_RENDERFX32)
		to->renderfx = MSG_ReadLong();
	else if (bits & U_RENDERFX8)
		to->renderfx = MSG_ReadByte();
	else if (bits & U_RENDERFX16)
		to->renderfx = MSG_ReadShort();

	if (bits & U_ORIGIN1) to->origin[0] = MSG_ReadCoord();
	if (bits & U_ORIGIN2) to->origin[1] = MSG_ReadCoord();
	if (bits & U_ORIGIN3) to->origin[2] = MSG_ReadCoord();

	if (flags & MSG_ES_SHORTANGLES && bits & U_ANGLE16) {
		if (bits & U_ANGLE1) to->angles[0] = MSG_ReadAngle16();
		if (bits & U_ANGLE2) to->angles[1] = MSG_ReadAngle16();
		if (bits & U_ANGLE3) to->angles[2] = MSG_ReadAngle16();
	} else {
		if (bits & U_ANGLE1) to->angles[0] = MSG_ReadAngle();
		if (bits & U_ANGLE2) to->angles[1] = MSG_ReadAngle();
		if (bits & U_ANGLE3) to->angles[2] = MSG_ReadAngle();
	}

	if (bits & U_OLDORIGIN)
		MSG_ReadPos(to->old_origin);

	if (bits & U_SOUND) {
		if (flags & MSG_ES_EXTENSIONS) {
			int w = MSG_ReadShort();
			to->sound = w & 0x3fff;
			if (w & 0x4000)
				ext->loop_volume = MSG_ReadByte() / 255.0f;
			if (w & 0x8000) {
				int b = MSG_ReadByte();
				if (b == 192)
					ext->loop_attenuation = ATTN_LOOP_NONE;
				else
					ext->loop_attenuation = b / 64.0f;
			}
		} else {
			to->sound = MSG_ReadByte();
		}
	}

	if (bits & U_EVENT)
		to->event = MSG_ReadByte();

	if (bits & U_SOLID) {
		if (flags & MSG_ES_LONGSOLID)
			to->solid = MSG_ReadLong();
		else
			to->solid = MSG_ReadShort();
	}

	if (flags & MSG_ES_EXTENSIONS) {
		if ((bits & U_MOREFX32) == U_MOREFX32)
			ext->morefx = MSG_ReadLong();
		else if (bits & U_MOREFX8)
			ext->morefx = MSG_ReadByte();
		else if (bits & U_MOREFX16)
			ext->morefx = MSG_ReadShort();

		if (bits & U_ALPHA)
			ext->alpha = MSG_ReadByte() / 255.0f;

		if (bits & U_SCALE)
			ext->scale = MSG_ReadByte() / 16.0f;
	}
}

void CG_ReadDeltaPlayerState(const player_state_t *from, player_state_t *to, msgPsFlags_t psflags)
{
	int         i;
	int         statbits;
	int			flags, extraflags;

	Q_assert(to);

	// clear to old value before delta parsing
	if (!from) {
		memset(to, 0, sizeof(*to));
	} else if (to != from) {
		memcpy(to, from, sizeof(*to));
	}

	flags = MSG_ReadWord();
	extraflags = MSG_ReadByte();

	//
	// parse the pmove_state_t
	//
	if (flags & PS_M_TYPE)
		to->pmove.pm_type = MSG_ReadByte();

	if (flags & PS_M_ORIGIN) {
		to->pmove.origin[0] = MSG_ReadShort();
		to->pmove.origin[1] = MSG_ReadShort();
	}

	if (extraflags & EPS_M_ORIGIN2)
		to->pmove.origin[2] = MSG_ReadShort();

	if (flags & PS_M_VELOCITY) {
		to->pmove.velocity[0] = MSG_ReadShort();
		to->pmove.velocity[1] = MSG_ReadShort();
	}

	if (extraflags & EPS_M_VELOCITY2)
		to->pmove.velocity[2] = MSG_ReadShort();

	if (flags & PS_M_TIME)
		to->pmove.pm_time = MSG_ReadByte();

	if (flags & PS_M_FLAGS)
		to->pmove.pm_flags = MSG_ReadByte();

	if (flags & PS_M_GRAVITY)
		to->pmove.gravity = MSG_ReadShort();

	if (flags & PS_M_DELTA_ANGLES) {
		to->pmove.delta_angles[0] = MSG_ReadShort();
		to->pmove.delta_angles[1] = MSG_ReadShort();
		to->pmove.delta_angles[2] = MSG_ReadShort();
	}

	//
	// parse the rest of the player_state_t
	//
	if (flags & PS_VIEWOFFSET) {
		to->viewoffset[0] = MSG_ReadChar() * 0.25f;
		to->viewoffset[1] = MSG_ReadChar() * 0.25f;
		to->viewoffset[2] = MSG_ReadChar() * 0.25f;
	}

	if (flags & PS_VIEWANGLES) {
		to->viewangles[0] = MSG_ReadAngle16();
		to->viewangles[1] = MSG_ReadAngle16();
	}

	if (extraflags & EPS_VIEWANGLE2)
		to->viewangles[2] = MSG_ReadAngle16();

	if (flags & PS_KICKANGLES) {
		to->kick_angles[0] = MSG_ReadChar() * 0.25f;
		to->kick_angles[1] = MSG_ReadChar() * 0.25f;
		to->kick_angles[2] = MSG_ReadChar() * 0.25f;
	}

	if (flags & PS_WEAPONINDEX) {
		if (psflags & MSG_PS_EXTENSIONS)
			to->gunindex = MSG_ReadWord();
		else
			to->gunindex = MSG_ReadByte();
	}

	if (flags & PS_WEAPONFRAME)
		to->gunframe = MSG_ReadByte();

	if (extraflags & EPS_GUNOFFSET) {
		to->gunoffset[0] = MSG_ReadChar() * 0.25f;
		to->gunoffset[1] = MSG_ReadChar() * 0.25f;
		to->gunoffset[2] = MSG_ReadChar() * 0.25f;
	}

	if (extraflags & EPS_GUNANGLES) {
		to->gunangles[0] = MSG_ReadChar() * 0.25f;
		to->gunangles[1] = MSG_ReadChar() * 0.25f;
		to->gunangles[2] = MSG_ReadChar() * 0.25f;
	}

	if (flags & PS_BLEND) {
		to->blend[0] = MSG_ReadByte() / 255.0f;
		to->blend[1] = MSG_ReadByte() / 255.0f;
		to->blend[2] = MSG_ReadByte() / 255.0f;
		to->blend[3] = MSG_ReadByte() / 255.0f;
	}

	if (flags & PS_FOV)
		to->fov = MSG_ReadByte();

	if (flags & PS_RDFLAGS)
		to->rdflags = MSG_ReadByte();

	// parse stats
	if (extraflags & EPS_STATS) {
		statbits = MSG_ReadLong();
		for (i = 0; i < MAX_STATS; i++)
			if (statbits & BIT(i))
				to->stats[i] = MSG_ReadShort();
	}
}



// prediction
static int pm_clipmask;
trace_t q_gameabi PM_trace(const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end)
{
	return gi.trace(start, mins, maxs, end, pm_clipmask);
}

void CG_RunPrediction(pmove_t *pm, int *o_current, int *o_ack, int *o_frame)
{
	pweapon_state_t pw;
	pm_clipmask = MASK_PLAYERSOLID;
	int current, ack, frame;
	current = *o_current;
	ack = *o_ack;
	frame = *o_frame;

	// remaster player collision rules
	if (cl->frame.ps.pmove.pm_type == PM_DEAD || cl->frame.ps.pmove.pm_type == PM_GIB)
		pm_clipmask = MASK_DEADSOLID;

	if (!(cl->frame.ps.pmove.pm_flags & PMF_IGNORE_PLAYER_COLLISION))
		pm_clipmask |= CONTENTS_PLAYER;

	// copy current state to pmove
	memset(pm, 0, sizeof(pmove_t));
	pm->trace = PM_trace;
	pm->pointcontents = gi.pointcontents;
	pm->s = cl->frame.ps.pmove;

	// run frames
	while (++ack <= current) {
		pm->cmd = cl->cmds[ack & CMD_MASK];
		Pmove(pm, &pw, &global_pmp);

		// save for debug checking
		VectorCopy(pm->s.origin, cl->predicted_origins[ack & CMD_MASK]);
	}

	// run pending cmd
	#if 1
	if (cl->cmd.msec) {
		pm->cmd = cl->cmd;
		pm->cmd.forwardmove = cl->localmove[0];
		pm->cmd.sidemove = cl->localmove[1];
		pm->cmd.upmove = cl->localmove[2];
		Pmove(pm, &pw, &global_pmp);
		frame = current;

		// save for debug checking
		VectorCopy(pm->s.origin, cl->predicted_origins[(current + 1) & CMD_MASK]);
	} else {
		frame = current - 1;
	}
	#endif


	*o_current = current;
	*o_ack = ack;
	*o_frame = frame;
}


// called after frame has been parsed
void CG_FinalizeFrame(void)
{
	int i = cl->frame.firstEntity;
	for(; i < (cl->frame.firstEntity + cl->frame.numEntities); i++)
	{
		int j = i & ENTITY_STATE_MASK;
		centity_state_t *ent = &cl->entityStates[j];

		if (ent->s.type == 0) // 0 type ents are treated like normal
			continue;

		
	}
}








