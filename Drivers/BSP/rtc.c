#include "rtc.h"
#include "ds3231m.h"
//extern RTC_HandleTypeDef hrtc;

RTC_DateTypeDef rtcDate;
RTC_TimeTypeDef rtcTime;
	
//void RTC_Set()
//{
//	RTC_DateTypeDef sdatestructure;
//  RTC_TimeTypeDef stimestructure;

//  sdatestructure.Year = 0x14;
//  sdatestructure.Month = RTC_MONTH_FEBRUARY;
//  sdatestructure.Date = 0x18;
//  sdatestructure.WeekDay = RTC_WEEKDAY_TUESDAY;
//  
//  HAL_RTC_SetDate(&hrtc,&sdatestructure,RTC_FORMAT_BCD);

//  stimestructure.Hours = 0x02;
//  stimestructure.Minutes = 0x00;
//  stimestructure.Seconds = 0x00;
//  stimestructure.TimeFormat = RTC_HOURFORMAT12_AM;
//  stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
//  stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;
//  
//  HAL_RTC_SetTime(&hrtc,&stimestructure,RTC_FORMAT_BCD);
//}

//void RTC_Get()
//{ 
//  HAL_RTC_GetTime(&hrtc, &rtcTime, RTC_FORMAT_BIN);
//  HAL_RTC_GetDate(&hrtc, &rtcDate, RTC_FORMAT_BIN);
//}

//void RTC_SetDateTime(RTC_DateTypeDef *sdatestructure, RTC_TimeTypeDef *stimestructure)
//{
//  HAL_RTC_SetDate(&hrtc,sdatestructure,RTC_FORMAT_BIN);
//  HAL_RTC_SetTime(&hrtc,stimestructure,RTC_FORMAT_BIN);
//}

void RTC_Init()
{
	DS3231M_Init();
	DS3231M_OsciCheck();
}

void RTC_Get()
{ 
  DS3231M_GetTime(&rtcDate,&rtcTime);
}

void RTC_SetDateTime(RTC_DateTypeDef *sdatestructure, RTC_TimeTypeDef *stimestructure)
{
  DS3231M_SetTime(sdatestructure,stimestructure);
}