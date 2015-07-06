#ifndef __RTC_H
#define __RTC_H

#include "stm32f4xx_hal.h"

extern RTC_DateTypeDef rtcDate;
extern RTC_TimeTypeDef rtcTime;

void RTC_Set();
void RTC_Get();
#endif

