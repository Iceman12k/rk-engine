
#include "cg_local.h"



void CG_UI_Render(vec2_t screensize)
{
	char buffer[256];

	Q_snprintf(buffer, sizeof(buffer), "%i", cl->servertime - cl->time);
	gx.R_DrawString(12, 12, UI_ALTCOLOR, MAX_STRING_CHARS, buffer);
	
	int yoff = sin((float)cl->time / 1000.0) * 100;
	gx.R_DrawStretchPic(12, 64 + yoff, 64, 64, "w_blaster.pcx");
}





















