#include "pwr.h"

void DP_EN(int val)
{
	HAL_GPIO_WritePin (GPIOE,GPIO_PIN_2,(GPIO_PinState)val);
}

void AP_EN(int val)
{
	HAL_GPIO_WritePin (GPIOI,GPIO_PIN_3,(GPIO_PinState)val);
}

void AMP_EN(int val)
{
	HAL_GPIO_WritePin (GPIOH,GPIO_PIN_15,(GPIO_PinState)val);
}

void MUX(int val)
{
	HAL_GPIO_WritePin (GPIOH,GPIO_PIN_13,(GPIO_PinState)val);
}