#include "geotest.h"
#include "pwr.h"
#include "dac8831.h"
#include "ad7190.h"
#include "GUI.h"
#include <stdio.h>
#include "bg2.c"

static float Inbuff[4096];

void geotest()
{
	int i;
	uint32_t color=0;
	char string[24];
	
	AP_EN(1);
	AMP_EN(1);
	MUX(0);
	//DAC_SET(0x7fff);
	DAC_SET(0x7fff+0xffff/5);
	osDelay(100);
//	AD7190_Reset();
	AD7190_Calibration();
	AD7190_Setup();
	//osDelay(200);
	//GUI_JPEG_Draw(_acbg2, sizeof(_acbg2), 0, 0);
	while(1)
	{
		if(color == 0)
			color = 0x001e29ce;
		else
			color = 0;
		GUI_SetColor(color);
		for(i=0;i<30;i++)
		{
			//sprintf(string," %04x ",AD7190Read());
			//GUI_DispStringAt(string,200,i*32);
			GUI_DispHexAt(AD7190Read(),0,i*48,4);
			//GUI_DispCEOL();
		}
		osDelay(1000);
	}

//	DAC_SWEEP(10,1,0);
}
