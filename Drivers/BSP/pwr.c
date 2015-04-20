#include "pwr.h"

void DP_EN(int val)
{
	HAL_GPIO_WritePin (GPIOE,GPIO_PIN_2,(GPIO_PinState)val);
}
