#include "dac8831.h"
#include "math.h"

TIM_HandleTypeDef htim2;
extern SPI_HandleTypeDef hspi6;
int POINTS = 300;
static uint16_t DataBuf[400];
static uint32_t index;

static void TIM2_Init(uint32_t period);
	
void DAC_SWEEP(uint32_t frequency,float mag,float offset)
{
	uint32_t i;
	uint32_t period;
	
	if(!frequency)
	{
		HAL_TIM_Base_Stop_IT(&htim2);
		HAL_TIM_Base_DeInit(&htim2);
		DAC_SET(DACMID);
	}
	else
	{
		index = 0;
		//POINTS = frequency<=80 ? 300 : frequency<=200 ? 100 : 50;
		period = 84000000.0/POINTS/frequency;
		for(i=0;i<POINTS;i++)
				DataBuf[i] = (uint16_t)(DACMID+offset*0xffff/5+mag/5*0xffff*sin(2*PI*i/POINTS));
		TIM2_Init(period);
		HAL_TIM_Base_Start_IT(&htim2);
	}
}

unsigned short DAC_SET(uint16_t code)
{
	uint8_t data[2];
	data[0] = code>>8;
	data[1] = code&0xff;
	HAL_GPIO_WritePin (GPIOG,GPIO_PIN_10,GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi6,data,2,200);
	HAL_GPIO_WritePin (GPIOG,GPIO_PIN_10,GPIO_PIN_SET);
}


void TIM2_Init(uint32_t period)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = period-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_Base_Init(&htim2);

//  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
//  HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig);

//  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
//  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
//  HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig);

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  DAC_SET(DataBuf[index++]);
	if(index==POINTS) index=0;
}
