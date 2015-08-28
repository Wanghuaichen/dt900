#include "ostask.h"
#include "rtc.h"
#include "ui.h"
#include "GUI.h"
#include "geotest.h"
#include "adc.h"
#include "beep.h"
#include "pwr.h"
#include "cmsis_os.h"
#include "ds18b20.h"
//#define _DEBUG

extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelvetica32;
extern struct Settings settings;
extern float curTemperature;
extern struct UIInfo UIInfo;

static int chargeSwap = 1;
static void batVolt();

void StartDefaultTask(void const * argument)
{
	char str[20];
	static unsigned int i=0;
#ifndef _DEBUG	
	reTemp();
#endif
	for(;;)
  {
#ifndef _DEBUG	
		RTC_Get();
		sprintf(str,"[ %02d:%02d  ",rtcTime.Hours, rtcTime.Minutes);
		GUI_SetColor(WHITE);
		GUI_SetBkColor(TITLECOLOR);
		GUI_SetFont(&GUI_FontHelvetica32);	
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_TOP);
		GUI_DispStringAt(str,20,0);
		
		if(chargeSwap)
		{
			GUI_SetTextAlign(GUI_TA_RIGHT | GUI_TA_TOP);
			GUI_DispStringAt(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_6)?"   ":"|",400,0);
			HAL_Delay(10);
			batVolt();
			chargeSwap=0;
		}
			
		if(i%120==0 && (settings.sensormode==1 || settings.sensormode==2))
		{
			curTemperature = DS18B20_Get_Temp();
			sprintf(str,"  ] %.1f~C  ",curTemperature);
			GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_TOP);
			GUI_DispStringAt(str,240,0);
		}
			
		if(i%300==0)
		{
			batVolt();
		}
		i++;
#endif
	osDelay(1000);
  }
}

void KeyTask(void const * argument)
{
#ifdef _DEBUG	
	int ret;
	GUI_SetColor(WHITE);
	GUI_FillRect(0,0,479,799);
	
	GUI_SetColor(BLACK);
	GUI_SetBkColor(WHITE);
	GUI_SetFont(&GUI_FontHelvetica32);	
	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_TOP);
#endif
  for(;;)
  {
		UIEventManager();
		osDelay(1);
  }
}

void dbg(char * str)
{
//	char s[80];
//	sprintf(s,"    %s    ",str);
//	GUI_SetColor(WHITE);
//	GUI_SetBkColor(TITLECOLOR);
//	GUI_SetFont(&GUI_FontHelvetica32);	
//	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_TOP);
//	GUI_DispStringAt(s,240,0);
}

void reTemp()
{
	char str[20];
	
	switch(settings.sensormode)
	{
		case 0:
			sprintf(str,"  ] OFF  ");
			break;
		case 3:
			sprintf(str,"  ] %.1f~C  ",settings.temperature);
			break;
		default:
			DS18B20_Get_Temp();
			HAL_Delay(200);
			curTemperature = DS18B20_Get_Temp();
			sprintf(str,"  ] %.1f~C  ",curTemperature);
			break;
	}
	GUI_SetColor(WHITE);
	GUI_SetBkColor(TITLECOLOR);
	GUI_SetFont(&GUI_FontHelvetica32);
	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_TOP);
	GUI_DispStringAt(str,240,0);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	chargeSwap = 1;
}

static void batVolt()
{
	static int batLevel = 0;
	int curLevel;
	float volt;
	volt = ADC_GetValue();
	curLevel = (volt-3.3)/0.08;
	if(curLevel>10)
		curLevel = 10;
	if(batLevel != curLevel)
	{
		batLevel = curLevel;
		GUI_SetColor(TITLECOLOR);
		GUI_FillRect(413,0,459,30);
		GUI_SetColor(WHITE);
		GUI_AA_DrawRoundedRect(413,6,459,25,3);
		if(curLevel>0)
			GUI_FillRect(416,9,416+4*batLevel,22);
	}
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
