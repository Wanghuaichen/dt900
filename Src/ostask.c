#include "ostask.h"
#include "rtc.h"
#include "ui.h"
#include "GUI.h"
#include "geotest.h"
#include "adc.h"
#include "beep.h"
#include "pwr.h"
#include "cmsis_os.h"

extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelvetica32;
extern struct GeoParam geoparam[10];
extern struct Settings settings;

void StartDefaultTask(void const * argument)
{
	char str[20];
	static int i=0;
	float volt;
	for(;;)
  {
		RTC_Get();
////		sprintf(str,"%02d:%02d:%02d %02d-%02d-%02d",
////								rtcTime.Hours, rtcTime.Minutes, rtcTime.Seconds,
////								rtcDate.Month, rtcDate.Date,rtcDate.Year);
		sprintf(str,"%02d:%02d",rtcTime.Hours, rtcTime.Minutes);
		GUI_SetColor(WHITE);
		GUI_SetBkColor(TITLECOLOR);
		GUI_SetFont(&GUI_FontHelvetica32);	
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_TOP);
		GUI_DispStringAt(str,0,0);
		
		if(i%300==0)
		{
			volt = ADC_GetValue();
			GUI_SetColor(WHITE);
			GUI_SetBkColor(TITLECOLOR);
			GUI_SetFont(&GUI_FontHelvetica32);	
			GUI_SetTextAlign(GUI_TA_RIGHT | GUI_TA_TOP);
			sprintf(str,"  %.1fV",volt);
			GUI_DispStringAt(str,479,0);
			if(volt<3.3)
			{
				beep(100);
				HAL_Delay(100);
				beep(100);
				HAL_Delay(100);
				beep(100);
				LCD_PWR(0);
				DP_EN(0);
			}
		}
		i++;
		osDelay(1000);
  }
}

void KeyTask(void const * argument)
{
	static uint32_t i = 0;
  for(;;)
  {
		UIKeyboard();
		UITouch();
		UIEventManager();
		osDelay(10);
  }
}