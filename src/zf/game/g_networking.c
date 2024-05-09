

#include "g_local.h"

const entity_packed_t    nullEntityState;
const player_packed_t    nullPlayerState;
const usercmd_t          nullUserCmd;

void GAME_WriteDeltaEntity(const entity_packed_t *from, const entity_packed_t *to, msgEsFlags_t flags)
{
	uint64_t    bits;
    uint32_t    mask;

    if (!to) {
        Q_assert(from);
        Q_assert(from->number > 0 && from->number < MAX_EDICTS);
		
        bits = U_REMOVE;
        if (from->number & 0xff00)
            bits |= U_NUMBER16 | U_MOREBITS1;

        MSG_WriteByte(bits & 255);
        if (bits & 0x0000ff00)
            MSG_WriteByte((bits >> 8) & 255);

        if (bits & U_NUMBER16)
            MSG_WriteShort(from->number);
        else
            MSG_WriteByte(from->number);

        return; // remove entity
    }

    Q_assert(to->number > 0 && to->number < MAX_EDICTS);

    if (!from)
        from = &nullEntityState;

// send an update
    bits = 0;
    if (!(flags & MSG_ES_FIRSTPERSON)) {
        if (to->origin[0] != from->origin[0])
            bits |= U_ORIGIN1;
        if (to->origin[1] != from->origin[1])
            bits |= U_ORIGIN2;
        if (to->origin[2] != from->origin[2])
            bits |= U_ORIGIN3;

        if (flags & MSG_ES_SHORTANGLES && to->solid == PACKED_BSP) {
            if (to->angles[0] != from->angles[0])
                bits |= U_ANGLE1 | U_ANGLE16;
            if (to->angles[1] != from->angles[1])
                bits |= U_ANGLE2 | U_ANGLE16;
            if (to->angles[2] != from->angles[2])
                bits |= U_ANGLE3 | U_ANGLE16;
        } else {
            if ((to->angles[0] ^ from->angles[0]) & 0xff00)
                bits |= U_ANGLE1;
            if ((to->angles[1] ^ from->angles[1]) & 0xff00)
                bits |= U_ANGLE2;
            if ((to->angles[2] ^ from->angles[2]) & 0xff00)
                bits |= U_ANGLE3;
        }

        if ((flags & MSG_ES_NEWENTITY) && !VectorCompare(to->old_origin, from->origin))
            bits |= U_OLDORIGIN;
    }

    if (flags & MSG_ES_UMASK)
        mask = 0xffff0000;
    else
        mask = 0xffff8000;  // don't confuse old clients

    if (to->skinnum != from->skinnum) {
        if (to->skinnum & mask)
            bits |= U_SKIN32;
        else if (to->skinnum & 0x0000ff00)
            bits |= U_SKIN16;
        else
            bits |= U_SKIN8;
    }

    if (to->frame != from->frame) {
        if (to->frame & 0xff00)
            bits |= U_FRAME16;
        else
            bits |= U_FRAME8;
    }

    if (to->effects != from->effects) {
        if (to->effects & mask)
            bits |= U_EFFECTS32;
        else if (to->effects & 0x0000ff00)
            bits |= U_EFFECTS16;
        else
            bits |= U_EFFECTS8;
    }

    if (to->renderfx != from->renderfx) {
        if (to->renderfx & mask)
            bits |= U_RENDERFX32;
        else if (to->renderfx & 0x0000ff00)
            bits |= U_RENDERFX16;
        else
            bits |= U_RENDERFX8;
    }

    if (to->solid != from->solid)
        bits |= U_SOLID;

    // event is not delta compressed, just 0 compressed
    if (to->event)
        bits |= U_EVENT;

    if (to->modelindex != from->modelindex)
        bits |= U_MODEL;
    if (to->modelindex2 != from->modelindex2)
        bits |= U_MODEL2;
    if (to->modelindex3 != from->modelindex3)
        bits |= U_MODEL3;
    if (to->modelindex4 != from->modelindex4)
        bits |= U_MODEL4;

    if (flags & MSG_ES_EXTENSIONS) {
        if (bits & (U_MODEL | U_MODEL2 | U_MODEL3 | U_MODEL4) &&
            (to->modelindex | to->modelindex2 | to->modelindex3 | to->modelindex4) & 0xff00)
            bits |= U_MODEL16;
        if (to->loop_volume != from->loop_volume || to->loop_attenuation != from->loop_attenuation)
            bits |= U_SOUND;
        if (to->morefx != from->morefx) {
            if (to->morefx & mask)
                bits |= U_MOREFX32;
            else if (to->morefx & 0x0000ff00)
                bits |= U_MOREFX16;
            else
                bits |= U_MOREFX8;
        }
        if (to->alpha != from->alpha)
            bits |= U_ALPHA;
        if (to->scale != from->scale)
            bits |= U_SCALE;
    }

    if (to->sound != from->sound)
        bits |= U_SOUND;

    if (to->renderfx & RF_FRAMELERP) {
        if (!VectorCompare(to->old_origin, from->origin))
            bits |= U_OLDORIGIN;
    } else if (to->renderfx & RF_BEAM) {
        if (!(flags & MSG_ES_BEAMORIGIN) || !VectorCompare(to->old_origin, from->old_origin))
            bits |= U_OLDORIGIN;
    }

    //
    // write the message
    //
    if (!bits && !(flags & MSG_ES_FORCE))
        return;     // nothing to send!

    if (flags & MSG_ES_REMOVE)
        bits |= U_REMOVE; // used for MVD stream only

    //----------

    if (to->number & 0xff00)
        bits |= U_NUMBER16;     // number8 is implicit otherwise

    if (bits & 0xff00000000ULL)
        bits |= U_MOREBITS4 | U_MOREBITS3 | U_MOREBITS2 | U_MOREBITS1;
    else if (bits & 0xff000000)
        bits |= U_MOREBITS3 | U_MOREBITS2 | U_MOREBITS1;
    else if (bits & 0x00ff0000)
        bits |= U_MOREBITS2 | U_MOREBITS1;
    else if (bits & 0x0000ff00)
        bits |= U_MOREBITS1;

    MSG_WriteByte(bits & 255);
    if (bits & U_MOREBITS1) MSG_WriteByte((bits >>  8) & 255);
    if (bits & U_MOREBITS2) MSG_WriteByte((bits >> 16) & 255);
    if (bits & U_MOREBITS3) MSG_WriteByte((bits >> 24) & 255);
    if (bits & U_MOREBITS4) MSG_WriteByte((bits >> 32) & 255);

    //----------

    if (bits & U_NUMBER16)
        MSG_WriteShort(to->number);
    else
        MSG_WriteByte(to->number);

    if (bits & U_MODEL16) {
        if (bits & U_MODEL ) MSG_WriteShort(to->modelindex);
        if (bits & U_MODEL2) MSG_WriteShort(to->modelindex2);
        if (bits & U_MODEL3) MSG_WriteShort(to->modelindex3);
        if (bits & U_MODEL4) MSG_WriteShort(to->modelindex4);
    } else {
        if (bits & U_MODEL ) MSG_WriteByte(to->modelindex);
        if (bits & U_MODEL2) MSG_WriteByte(to->modelindex2);
        if (bits & U_MODEL3) MSG_WriteByte(to->modelindex3);
        if (bits & U_MODEL4) MSG_WriteByte(to->modelindex4);
    }

    if (bits & U_FRAME8)
        MSG_WriteByte(to->frame);
    else if (bits & U_FRAME16)
        MSG_WriteShort(to->frame);

    if ((bits & U_SKIN32) == U_SKIN32)
        MSG_WriteLong(to->skinnum);
    else if (bits & U_SKIN8)
        MSG_WriteByte(to->skinnum);
    else if (bits & U_SKIN16)
        MSG_WriteShort(to->skinnum);

    if ((bits & U_EFFECTS32) == U_EFFECTS32)
        MSG_WriteLong(to->effects);
    else if (bits & U_EFFECTS8)
        MSG_WriteByte(to->effects);
    else if (bits & U_EFFECTS16)
        MSG_WriteShort(to->effects);

    if ((bits & U_RENDERFX32) == U_RENDERFX32)
        MSG_WriteLong(to->renderfx);
    else if (bits & U_RENDERFX8)
        MSG_WriteByte(to->renderfx);
    else if (bits & U_RENDERFX16)
        MSG_WriteShort(to->renderfx);

    if (bits & U_ORIGIN1) MSG_WriteShort(to->origin[0]);
    if (bits & U_ORIGIN2) MSG_WriteShort(to->origin[1]);
    if (bits & U_ORIGIN3) MSG_WriteShort(to->origin[2]);

    if (bits & U_ANGLE16) {
        if (bits & U_ANGLE1) MSG_WriteShort(to->angles[0]);
        if (bits & U_ANGLE2) MSG_WriteShort(to->angles[1]);
        if (bits & U_ANGLE3) MSG_WriteShort(to->angles[2]);
    } else {
        if (bits & U_ANGLE1) MSG_WriteChar(to->angles[0] >> 8);
        if (bits & U_ANGLE2) MSG_WriteChar(to->angles[1] >> 8);
        if (bits & U_ANGLE3) MSG_WriteChar(to->angles[2] >> 8);
    }

    if (bits & U_OLDORIGIN) {
        MSG_WriteShort(to->old_origin[0]);
        MSG_WriteShort(to->old_origin[1]);
        MSG_WriteShort(to->old_origin[2]);
    }

    if (bits & U_SOUND) {
        if (flags & MSG_ES_EXTENSIONS) {
            int w = to->sound & 0x3fff;

            if (to->loop_volume != from->loop_volume)
                w |= 0x4000;
            if (to->loop_attenuation != from->loop_attenuation)
                w |= 0x8000;

            MSG_WriteShort(w);
            if (w & 0x4000)
                MSG_WriteByte(to->loop_volume);
            if (w & 0x8000)
                MSG_WriteByte(to->loop_attenuation);
        } else {
            MSG_WriteByte(to->sound);
        }
    }

    if (bits & U_EVENT)
        MSG_WriteByte(to->event);

    if (bits & U_SOLID) {
        if (flags & MSG_ES_LONGSOLID)
            MSG_WriteLong(to->solid);
        else
            MSG_WriteShort(to->solid);
    }

    if ((bits & U_MOREFX32) == U_MOREFX32)
        MSG_WriteLong(to->morefx);
    else if (bits & U_MOREFX8)
        MSG_WriteByte(to->morefx);
    else if (bits & U_MOREFX16)
        MSG_WriteShort(to->morefx);

    if (bits & U_ALPHA)
        MSG_WriteByte(to->alpha);

    if (bits & U_SCALE)
        MSG_WriteByte(to->scale);
}







