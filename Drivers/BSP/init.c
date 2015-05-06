#include "init.h"
#include "pwr.h"
#include "adc.h"
#include "beep.h"
#include "sdram.h"
#include "lcd.h"

void Board_Init()
{
//	DP_EN(0);
//	HAL_Delay(300);
//	if(ADC_GetValue()<3.3)
//	{
//		beep(100);
//		HAL_Delay(100);
//		beep(100);
//		HAL_Delay(100);
//		beep(100);
//		while(1);
//	}
//	else
	{
		beep(500);
		DP_EN(1);
		Sdram_Init();
	}
}
