#include "beep.h"

extern TIM_HandleTypeDef htim2;

void beep(uint32_t ms)
{
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	HAL_Delay(ms);
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
}
