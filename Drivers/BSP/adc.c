#include "adc.h"

extern ADC_HandleTypeDef hadc1;

double ADC_GetValue(void)
{
	double ret;

	//NMOS On
	HAL_GPIO_WritePin (GPIOC,GPIO_PIN_2,GPIO_PIN_SET);
	
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1,200);
	ret = HAL_ADC_GetValue (&hadc1);
	HAL_ADC_Stop(&hadc1);
	//NMOS Off
	HAL_GPIO_WritePin (GPIOC,GPIO_PIN_2,GPIO_PIN_RESET);
	
	return ret*4/3/4096*3.3;
}
