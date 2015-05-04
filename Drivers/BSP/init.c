#include "init.h"
#include "pwr.h"
#include "adc.h"
#include "beep.h"
#include "sdram.h"

void Board_Init()
{
	DP_EN(0);
	HAL_Delay(300);
	if(ADC_GetValue()<3.3)
	{
		beep(100);
		HAL_Delay(100);
		beep(100);
		HAL_Delay(100);
		beep(100);
		while(1);
	}
	else
	{
		beep(500);
		DP_EN(1);
//		Sdram_Init();
//		LCD_PWR(1);
//		LCD_LayerCfg();
//		memset((char*)0xD0000000,0,0x800000);
//		memset((char*)(0xD0000000+800*3*240),0xff,800*3*40);
	}
}
