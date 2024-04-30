
#include "g_local.h"

void SP_worldspawn(edict_t *ent)
{
	ent->solid = SOLID_BSP;
	ent->inuse = true;          // since the world doesn't use G_Spawn()
	ent->s.modelindex = 1;      // world model is always index 1

	// make some data visible to the server

	
	if (ent->message && ent->message[0]) {
		gi.configstring(CS_NAME, ent->message);
		Q_strlcpy(level.level_name, ent->message, sizeof(level.level_name));
	}
	else
		Q_strlcpy(level.level_name, level.mapname, sizeof(level.level_name));

	if (st.sky && st.sky[0])
		gi.configstring(CS_SKY, st.sky);
	else
		gi.configstring(CS_SKY, "unit1_");

	gi.configstring(CS_SKYROTATE, va("%f", st.skyrotate));

	gi.configstring(CS_SKYAXIS, va("%f %f %f",
		st.skyaxis[0], st.skyaxis[1], st.skyaxis[2]));

	gi.configstring(game.csr.maxclients, va("%i", (int)(maxclients->value)));

	//---------------

	// precache assets
	gi.modelindex("models/objects/gibs/head2/tris.md2"); // test
	gi.modelindex("models/null.md2");
	gi.modelindex("models/hud/bar.md2");
	gi.modelindex("models/weapons/v_pickaxe.md2");
	gi.modelindex("models/inven/square.md2");
	gi.modelindex("models/inven/i_pickaxe.md2");

	gi.modelindex("models/monsters/necro.md2");
	gi.modelindex("models/monsters/wraith.md2");
	gi.modelindex("models/monsters/torment.md2");

	//gi.imageindex("infoblock");
	//gi.imageindex("hotbar");
	//gi.imageindex("hotbars");
	//gi.imageindex("hotbarc");

	gi.soundindex("western/hammer_hit.wav");
	gi.soundindex("western/pick_hit.wav");
	gi.soundindex("western/swing_small.wav");
	gi.soundindex("western/swing_big.wav");
	gi.soundindex("western/switch_weapon.wav");
	gi.soundindex("western/deflect.wav");

#if 0
#if 1
	for (int i = -256; i <= 256; i += 32)
	{
		for (int j = -256; j <= 256; j += 32)
		{
			for (int k = -32; k <= 128; k += 32)
			{
				detail_edict_t *head = D_Spawn();
				head->s.modelindex = gi.modelindex("models/objects/gibs/head2/tris.md2");
				head->classname = "detail_head";
				VectorSet(head->s.origin, i, j, k);
			}
		}
	}
#elif 1
	for (int k = -128; k <= 128; k += 32)
	{
		detail_edict_t *head = D_Spawn();
		head->s.modelindex = gi.modelindex("models/objects/gibs/head2/tris.md2");
		head->classname = "detail_head";
		VectorSet(head->s.origin, 0, k, 0);
	}
#else
	detail_edict_t *head = D_Spawn();
	head->s.modelindex = gi.modelindex("models/objects/gibs/head2/tris.md2");
	head->classname = "detail_head";
#endif
#endif


	//
	// Setup light animation tables. 'a' is total darkness, 'z' is doublebright.
	//

	// 0 normal
	gi.configstring(game.csr.lights + 0, "m");

	// 1 FLICKER (first variety)
	gi.configstring(game.csr.lights + 1, "mmnmmommommnonmmonqnmmo");

	// 2 SLOW STRONG PULSE
	gi.configstring(game.csr.lights + 2, "abcdefghijklmnopqrstuvwxyzyxwvutsrqponmlkjihgfedcba");

	// 3 CANDLE (first variety)
	gi.configstring(game.csr.lights + 3, "mmmmmaaaaammmmmaaaaaabcdefgabcdefg");

	// 4 FAST STROBE
	gi.configstring(game.csr.lights + 4, "mamamamamama");

	// 5 GENTLE PULSE 1
	gi.configstring(game.csr.lights + 5, "jklmnopqrstuvwxyzyxwvutsrqponmlkj");

	// 6 FLICKER (second variety)
	gi.configstring(game.csr.lights + 6, "nmonqnmomnmomomno");

	// 7 CANDLE (second variety)
	gi.configstring(game.csr.lights + 7, "mmmaaaabcdefgmmmmaaaammmaamm");

	// 8 CANDLE (third variety)
	gi.configstring(game.csr.lights + 8, "mmmaaammmaaammmabcdefaaaammmmabcdefmmmaaaa");

	// 9 SLOW STROBE (fourth variety)
	gi.configstring(game.csr.lights + 9, "aaaaaaaazzzzzzzz");

	// 10 FLUORESCENT FLICKER
	gi.configstring(game.csr.lights + 10, "mmamammmmammamamaaamammma");

	// 11 SLOW PULSE NOT FADE TO BLACK
	gi.configstring(game.csr.lights + 11, "abcdefghijklmnopqrrqponmlkjihgfedcba");

	// styles 32-62 are assigned by the light program for switchable lights

	// 25 testing
	gi.configstring(game.csr.lights + 25, "m");
}

void SP_func_illusionary(edict_t *self)
{
	gi.setmodel(self, self->model);
	self->solid = SOLID_NOT;

	gi.linkentity(self);
}

