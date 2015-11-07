#include "stm32f4xx_hal.h"
#ifndef __DS3231M_H
#define __DS3231M_H
void DS3231M_OsciCheck();
void DS3231M_Init();
void DS3231M_SetTime(RTC_DateTypeDef *rtcDate,RTC_TimeTypeDef *rtcTime);
void DS3231M_GetTime(RTC_DateTypeDef *rtcDate,RTC_TimeTypeDef *rtcTime);
float DS3231M_GetTemp();
#endif