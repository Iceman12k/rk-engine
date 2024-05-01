
#include "cg_local.h"



void CG_UI_Render(vec2_t screensize)
{
	char buffer[256];

	Q_snprintf(buffer, sizeof(buffer), "%.0f, %.0f", screensize[0], screensize[1]);
	gx.R_DrawString(12, 12, UI_ALTCOLOR, MAX_STRING_CHARS, buffer);
	gx.R_DrawStretchPic(12, 64, 64, 64, "w_blaster.pcx");

}





















