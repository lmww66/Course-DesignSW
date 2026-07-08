#include "common.h"

float BatteryVol = 3.7f;

void drawBattery()
{
	const byte* battIcon;
	if(BatteryVol < 3.2f)
		battIcon = battIconEmpty;
	else if(BatteryVol < 3.7f)
		battIcon = battIconLow;
	else if(BatteryVol < 4.0f)
		battIcon = battIconHigh;
	else
		battIcon = battIconFull;

	draw_bitmap(0, FRAME_HEIGHT - 8, battIcon, 16, 8, NOINVERT, 0);
}