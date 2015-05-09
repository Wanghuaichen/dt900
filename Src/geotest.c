#include "geotest.h"
#include "pwr.h"
#include "dac8831.h"
#include "ad7190.h"
#include "GUI.h"

static float Inbuff[4096];

void geotest()
{
	int i;
	char string[20];
	uint32_t color=GUI_LIGHTBLUE;
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
	
	while(1)
	{
//	DAC_SET(0x7fff);
//	HAL_Delay(3000);
//	DAC_SET(0x7fff+0xffff/5);
//	HAL_Delay(3000);
		if(color == GUI_LIGHTBLUE)
			color = GUI_RED;
		else
			color = GUI_LIGHTBLUE;
		GUI_SetColor(color);
		for(i=0;i<20;i++)
			Inbuff[i] = AD7190Read();
		//sprintf(string," %06x ",AD7190Read());
		for(i=0;i<20;i++)
		{
			sprintf(string," 0x%04x ",(uint16_t)Inbuff[i]);
			GUI_DispStringAt(string,240,0+i*20);
		}
		//GUI_DispStringHCenterAt("0x000000",240,300);
		//GUI_DispHexAt(0,50,200,8);
		//GUI_DispHexAt(AD7190Read(),50,200,8);
		osDelay(1);
	}
//	DAC_SWEEP(10,1,0);
}

		//GUI_DispStringHCenterAt("Hello world!",240,300);
