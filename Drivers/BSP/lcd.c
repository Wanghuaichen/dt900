#include "lcd.h"

extern LTDC_HandleTypeDef hltdc;
extern TIM_HandleTypeDef htim5;
LTDC_LayerCfgTypeDef pLayerCfg1;
LTDC_LayerCfgTypeDef pLayerCfg2;
	
void LCD_PWR(int val)//1-on  0-off
{
	if(val)
	{
		HAL_GPIO_WritePin (GPIOI,GPIO_PIN_1,GPIO_PIN_SET);
		HAL_Delay(120);
		HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_4);
	}
	else
	{
		HAL_TIM_PWM_Stop(&htim5, TIM_CHANNEL_4);
		HAL_Delay(120);
		HAL_GPIO_WritePin (GPIOI,GPIO_PIN_1,GPIO_PIN_RESET);
	}
}
	
void LCD_BL(int val)
{
	TIM_OC_InitTypeDef sConfigOC;
	
	if(val<10 || val>90)
		return;

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = val;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  HAL_TIM_PWM_ConfigChannel(&htim5, &sConfigOC, TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_4);
}

void LCD_LayerCfg()
{
	pLayerCfg1.WindowX0 = 0;
  pLayerCfg1.WindowX1 = 800;
  pLayerCfg1.WindowY0 = 0;
  pLayerCfg1.WindowY1 = 480;
  pLayerCfg1.PixelFormat = LTDC_PIXEL_FORMAT_RGB888;
  pLayerCfg1.FBStartAdress = FB_LAYER1;
  pLayerCfg1.Alpha = 0xff;
  pLayerCfg1.Alpha0 = 0;
  pLayerCfg1.Backcolor.Blue = 0;
  pLayerCfg1.Backcolor.Green = 0;
  pLayerCfg1.Backcolor.Red = 0;
  pLayerCfg1.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
  pLayerCfg1.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
  pLayerCfg1.ImageWidth = 800;
  pLayerCfg1.ImageHeight = 480; 
	HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg1, 0);
	
//	pLayerCfg2.WindowX0 = 0;
//  pLayerCfg2.WindowX1 = 800;
//  pLayerCfg2.WindowY0 = 0;
//  pLayerCfg2.WindowY1 = 440;
//  pLayerCfg2.PixelFormat = LTDC_PIXEL_FORMAT_RGB888;
//  pLayerCfg2.FBStartAdress = FB_LAYER2;
//  pLayerCfg2.Alpha = 0xff;
//  pLayerCfg2.Alpha0 = 0;
//  pLayerCfg2.Backcolor.Blue = 0;
//  pLayerCfg2.Backcolor.Green = 0xff;
//  pLayerCfg2.Backcolor.Red = 0;
//  pLayerCfg2.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
//  pLayerCfg2.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
//  pLayerCfg2.ImageWidth = 800;
//  pLayerCfg2.ImageHeight = 480;
//	HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg2, 1);
}
	