#include "init.h"
#include "pwr.h"
#include "adc.h"
#include "beep.h"
#include "sdram.h"
#include "lcd.h"
#include "spiflash.h"
#include "GUI.h"

void Board_Init()
{
//	DP_EN(0);
//	HAL_Delay(300);
//	if(ADC_GetValue()<3.3)
//	{
//		beep(100);
//		HAL_Delay(100);
//		beep(100);
//		HAL_Delay(100);
//		beep(100);
//		while(1);
//	}
//	else
	{
		beep(500);
		DP_EN(1);
		Sdram_Init();
		SPI_FLASH_CS(1);
		HAL_GPIO_WritePin (GPIOG,GPIO_PIN_10,GPIO_PIN_SET);//spi6-dac
		
		GUI_Init();
		GUI_SetFont(GUI_FONT_COMIC24B_ASCII);
		GUI_SetBkColor(GUI_WHITE);
		GUI_Clear();
		GUI_SetColor(GUI_LIGHTBLUE);
		
  //MX_FATFS_Init();
  //MX_USB_DEVICE_Init();
	}
}
