#ifndef __RTC_H
#define __RTC_H

#include "stm32f4xx_hal.h"

void RTC_Init();

void RTC_Get();

void RTC_SetDateTime(RTC_DateTypeDef *sdatestructure, RTC_TimeTypeDef *stimestructure);
#endif

