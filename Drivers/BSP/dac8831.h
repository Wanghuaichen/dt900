#ifndef __DAC8831_H
#define __DAC8831_H
#include "stm32f4xx_hal.h"

#define DACMID 0x7fff
#define POINTS 400
#define PI 3.1415926

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
unsigned short DAC_SET(uint16_t code);
void DAC_SWEEP(uint32_t frequency,float mag,float offset);
#endif
