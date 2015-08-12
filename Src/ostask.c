#include "ostask.h"
#include "rtc.h"
#include "ui.h"
#include "GUI.h"
#include "geotest.h"
#include "adc.h"
#include "beep.h"
#include "pwr.h"
#include "cmsis_os.h"
#include "cyma568.h"
#include "ds18b20.h"
//#define _DEBUG

extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelvetica32;
extern struct GeoParam geoparam[10];
extern struct Settings settings;

void StartDefaultTask(void const * argument)
{
	char str[20];
	static int i=0;
	float volt;
#ifdef _DEBUG	
	GUI_SetColor(WHITE);
	GUI_FillRect(0,0,479,799);
#endif
	for(;;)
  {
		
		
#ifndef _DEBUG	
		RTC_Get();
		sprintf(str,"%02d:%02d",rtcTime.Hours, rtcTime.Minutes);
		GUI_SetColor(WHITE);
		GUI_SetBkColor(TITLECOLOR);
		GUI_SetFont(&GUI_FontHelvetica32);	
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_TOP);
		GUI_DispStringAt(str,0,0);
		
		if(i%120==0 && (settings.sensormode==1 || settings.sensormode==2))
		{
			sprintf(str," %.1f~C ",DS18B20_Get_Temp());
			GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_TOP);
			GUI_DispStringAt(str,240,0);
		}
			
		if(i%300==0)
		{
			volt = ADC_GetValue();	
			sprintf(str,"  %.1fV",volt);
			GUI_SetTextAlign(GUI_TA_RIGHT | GUI_TA_TOP);
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
#endif


	osDelay(1000);
  }
}

void KeyTask(void const * argument)
{
	static uint32_t i = 0;
  for(;;)
  {
#ifdef _DEBUG
		touchreport();
#endif
		UIKeyboard();
		UITouch();
		UIEventManager();
		osDelay(10);
  }
}

void dbg(char * str)
{
	char s[80];
	sprintf(s,"    %s    ",str);
	GUI_SetColor(WHITE);
	GUI_SetBkColor(TITLECOLOR);
	GUI_SetFont(&GUI_FontHelvetica32);	
	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_TOP);
	GUI_DispStringAt(s,240,0);
}


