#include "rtc.h"

extern RTC_HandleTypeDef hrtc;

void RTC_Set()
{
	RTC_DateTypeDef sdatestructure;
  RTC_TimeTypeDef stimestructure;

  sdatestructure.Year = 0x14;
  sdatestructure.Month = RTC_MONTH_FEBRUARY;
  sdatestructure.Date = 0x18;
  sdatestructure.WeekDay = RTC_WEEKDAY_TUESDAY;
  
  HAL_RTC_SetDate(&hrtc,&sdatestructure,RTC_FORMAT_BCD);

  stimestructure.Hours = 0x02;
  stimestructure.Minutes = 0x00;
  stimestructure.Seconds = 0x00;
  stimestructure.TimeFormat = RTC_HOURFORMAT12_AM;
  stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;
  
  HAL_RTC_SetTime(&hrtc,&stimestructure,RTC_FORMAT_BCD);
}

void RTC_Get(char* str)
{
	RTC_DateTypeDef sdatestructureget;
  RTC_TimeTypeDef stimestructureget;
  
  HAL_RTC_GetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &sdatestructureget, RTC_FORMAT_BIN);
  sprintf(str,"%02d:%02d:%02d %02d-%02d-%02d",stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds,
	sdatestructureget.Month, sdatestructureget.Date,sdatestructureget.Year);
}

