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
#include "flash.h"
#include "ds3231m.h"

extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelvetica32;
extern struct Settings settings;
extern float curTemperature;
extern struct UIInfo UIInfo;
extern struct UIPage pAbout;
extern RTC_DateTypeDef rtcDate;
extern RTC_TimeTypeDef rtcTime;
extern IWDG_HandleTypeDef IwdgHandle;

volatile static int chargeSwap;
static int usbflag = 0;
static void batVolt();

void StartDefaultTask(void const * argument)
{
	char str[20];
	static volatile unsigned int i=0;
	
	for(;;)
  {
		if(UIInfo.PagePtr == &pAbout)
		{
			HAL_IWDG_Refresh(&IwdgHandle);
			osDelay(1000);
			continue;
		}
		else if(usbflag)
		{
			usbflag=0;
			i=0;
		}
		
		
		GUI_SetColor(WHITE);
		GUI_SetBkColor(TITLECOLOR);
		GUI_SetFont(&GUI_FontHelvetica32);	
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_TOP);
		
		RTC_Get();
		sprintf(str,"[ %02d:%02d  ",rtcTime.Hours, rtcTime.Minutes);
		GUI_DispStringAt(str,20,0);
			
		if(i%120==0)
		{
			reTemp();
		}
			
		if(i%300==0 || chargeSwap)
		{
			GUI_SetTextAlign(GUI_TA_RIGHT | GUI_TA_TOP);
			GUI_DispStringAt(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_6)?"   ":"|",400,0);
			HAL_Delay(10);
			batVolt();
			chargeSwap=0;
		}
		i++;
		
		HAL_IWDG_Refresh(&IwdgHandle);
		osDelay(1000);
  }
}

void KeyTask(void const * argument)
{
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
			sprintf(str,"   ] OFF   ");
			break;
		case 1:
			curTemperature = DS3231M_GetTemp();
			if(settings.units)
				sprintf(str,"  ] %.1f~F  ",(int)((curTemperature*1.8+32)*10+0.1)/10.0);
			else
				sprintf(str,"  ] %.1f~C  ",(int)(curTemperature*10+0.1)/10.0);
			break;
		case 2:
			curTemperature = 85;
			DS18B20_Get_Temp();
			while(curTemperature==85)
			{
				osDelay(800);
				curTemperature = DS18B20_Get_Temp();
			}
			if(settings.units)
				sprintf(str,"  ] %.1f~F  ",(int)((curTemperature*1.8+32)*10+0.1)/10.0);
			else
				sprintf(str,"  ] %.1f~C  ",(int)(curTemperature*10+0.1)/10.0);
			break;
		case 3:
			if(settings.units)
				sprintf(str,"  ] %.1f~F  ",(int)((settings.temperature*1.8+32)*10+0.1)/10.0);
			else
				sprintf(str,"  ] %.1f~C  ",(int)(settings.temperature*10+0.1)/10.0);
			break;
		case 4:
			FlashEraseTotal24();
			break;
		default:
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

int getbatvolt()
{
	float volt;
	int level;
	volt = ADC_GetValue();
	level = (volt-3.3)/0.16;
	return level>5? 5 : level<0 ? 0 : level;
}

static void batVolt()
{
	static int batLevel=0;
	int temp;
	int curLevel;
	
	curLevel = getbatvolt();
//	GUI_DispDecAt(volt*1000,0,750,4);	
	while(curLevel!=batLevel)
	{
		temp = curLevel;
		curLevel = getbatvolt();
		if(curLevel==temp)
			batLevel = curLevel;
	}
	
	GUI_SetColor(TITLECOLOR);
	GUI_FillRect(413,0,459,30);
	if(batLevel>1)
	{
		GUI_SetColor(WHITE);
		GUI_AA_DrawRoundedRect(413,6,459,25,3);
		GUI_FillRect(416,9,416+8*curLevel,22);
	}
	else
	{		
		GUI_SetColor(0xe4);
		GUI_AA_DrawRoundedRect(413,6,459,25,3);
		GUI_FillRect(416,9,416+8*curLevel,22);
		beep(300);
		GUI_SetColor(WHITE);
		GUI_AA_DrawRoundedRect(413,6,459,25,3);
		GUI_FillRect(416,9,416+8*curLevel,22);
		HAL_Delay(100);
		GUI_SetColor(0xe4);
		GUI_AA_DrawRoundedRect(413,6,459,25,3);
		GUI_FillRect(416,9,416+8*curLevel,22);
		beep(300);
		GUI_SetColor(WHITE);
		GUI_AA_DrawRoundedRect(413,6,459,25,3);
		GUI_FillRect(416,9,416+8*curLevel,22);
		HAL_Delay(100);
		GUI_SetColor(0xe4);
		GUI_AA_DrawRoundedRect(413,6,459,25,3);
		GUI_FillRect(416,9,416+8*curLevel,22);

		if(batLevel==0)
		{
			LCD_PWR(0);
			FlashProgram();
			DP_EN(0);
		}
	}
}

void usbflagchange()
{
	usbflag=1;
}