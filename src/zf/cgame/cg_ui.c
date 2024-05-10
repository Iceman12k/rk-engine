
#include "cg_local.h"



void CG_UI_Render(vec2_t screensize)
{
	char buffer[256];

	Q_snprintf(buffer, sizeof(buffer), "%i", cl->servertime - cl->time);
	gx.R_DrawString(12, 12, UI_ALTCOLOR, MAX_STRING_CHARS, buffer);

	// draw armor
	int xoff = 128;
	int yoff = screensize[1] - 128;
	/*
	gx.R_DrawStretchPic(xoff, yoff, 32, 32, "armor/ap_green_4");
	xoff += 22; gx.R_DrawStretchPic(xoff, yoff, 32, 32, "armor/ap_green_4");
	xoff += 22; gx.R_DrawStretchPic(xoff, yoff, 32, 32, "armor/ap_yellow_4");
	xoff += 22; gx.R_DrawStretchPic(xoff, yoff, 32, 32, "armor/ap_red_4");
	xoff += 22; gx.R_DrawStretchPic(xoff, yoff, 32, 32, "armor/ap_red_4");
	xoff += 22; gx.R_DrawStretchPic(xoff, yoff, 32, 32, "armor/ap_blue_3");
	*/
	
	static int bounce_timer = 0;
	int ap_remaining = cl->frame.ps.stats[STAT_ARMOR];
	int old_ap_remaining = cl->oldframe.ps.stats[STAT_ARMOR];

	const int BOUNCE_LENGTH = 110;
	if (ap_remaining != old_ap_remaining)
	{
		bounce_timer = cl->time + BOUNCE_LENGTH;
		//bounce_timer = max(bounce_timer, cl->time);
		//bounce_timer += BOUNCE_LENGTH;
	}
	
	const char *shieldname;
	int shield_num = 0;
	if (ap_remaining > 0)
	{
		while(ap_remaining > 0)
		{
			int ap = min(ap_remaining, AP_PER_SHIELD);
			ap_remaining -= ap;

			if (shield_num < SHIELDS_GREEN)
				shieldname = "green";
			else if (shield_num < SHIELDS_GREEN + SHIELDS_YELLOW)
				shieldname = "yellow";
			else if (shield_num < SHIELDS_GREEN + SHIELDS_YELLOW + SHIELDS_RED)
				shieldname = "red";
			else
				shieldname = "blue";
			
			int y_bounce = 0;
			if (ap_remaining <= AP_PER_SHIELD && cl->time < bounce_timer)
			{
				float strength = bounce_timer - cl->time;
				strength = (BOUNCE_LENGTH - strength);
				strength = 1 - (strength / BOUNCE_LENGTH);
				
				if (ap_remaining > 0)
					strength *= 0.3;
				y_bounce -= 12 * strength;
				y_bounce -= 14 * max(0.0, strength - 0.8);
				y_bounce -= 8 * max(0.0, strength - 0.5);
			}

			Q_snprintf(buffer, sizeof(buffer), "armor/ap_%s_%i", shieldname, ap);
			gx.R_DrawStretchPic(xoff, yoff + y_bounce, 32, 32, buffer);

			xoff += 22;
			shield_num++;
			
		}
	}
	else
	{
		gx.R_DrawStretchPic(xoff, yoff, 32, 32, "armor/ap_empty");
	}


	// draw health
	#define HP_BAR_LENGTH 135
	xoff = 128;
	yoff = screensize[1] - 92;
	gx.R_DrawStretchPic(xoff, yoff, HP_BAR_LENGTH, 32, "hp/healthbar_back");
	Q_snprintf(buffer, sizeof(buffer), "%i", (int)cl->frame.ps.stats[STAT_HEALTH]);
	gx.R_DrawString(xoff, yoff + 14, UI_DROPSHADOW | (cl->frame.ps.stats[STAT_HEALTH] > 100 ? UI_ALTCOLOR : 0), MAX_STRING_CHARS, buffer);

	float hp_frac_raw = (float)cl->frame.ps.stats[STAT_HEALTH] / 100.0;
	float hp_draw = bound(0, hp_frac_raw, 1);
	if (hp_draw > 0)
		hp_draw = max(hp_draw, 0.025);
	if (hp_draw < 1.0)
		hp_draw = min(hp_draw, 0.95);
	gx.R_SetClipArea(xoff, yoff, (HP_BAR_LENGTH * hp_draw), 32);
	gx.R_DrawStretchPic(xoff, yoff, HP_BAR_LENGTH, 32, "hp/healthbar_bar");

	hp_draw = bound(0, hp_frac_raw - 1.0, 1);
	if (hp_draw <= 1.0)
	{
		gx.R_SetClipArea(xoff, yoff, (HP_BAR_LENGTH * hp_draw), 32);
		gx.R_DrawStretchPic(xoff, yoff, HP_BAR_LENGTH, 32, "hp/healthbar_over");
	}
	gx.R_ResetClipArea();

	cl->frame.ps.gunindex = gi.modelindex("models/weapons/v_hyperb/tris.md2");
	cl->frame.ps.gunframe = 6;
}





















