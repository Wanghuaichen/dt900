#include "beep.h"

extern TIM_HandleTypeDef htim3;

void beep(uint32_t ms)
{
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
	HAL_Delay(ms);
	HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
}

void beepon()
{
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
}

void beepoff()
{
	HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
}