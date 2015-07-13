#include "init.h"
#include "pwr.h"
#include "adc.h"
#include "beep.h"
#include "sdram.h"
#include "lcd.h"
#include "spiflash.h"
#include "GUI.h"
#include "geotest.h"
#include "flash.h"

extern struct GeoParam geoparam[10];//(struct GeoParam *)PARAMADDR;
extern struct Settings settings;//(struct Settings *)SETADDR;

void Board_Init()
{
	DP_EN(0);
	if(ADC_GetValue()<3.4)
	{
		beep(100);
		HAL_Delay(100);
		beep(100);
		HAL_Delay(100);
		beep(100);
		while(1);
	}
	
	HAL_Delay(500);
	DP_EN(1);
	beepon();
	Sdram_Init();
	SPI_FLASH_CS(1);
	HAL_GPIO_WritePin (GPIOG,GPIO_PIN_10,GPIO_PIN_SET);//spi6-dac
	GUI_Init();
	UI_Init();
	LCD_PWR(1);
	beepoff();
	memcpy(&settings,(struct Settings *)SETADDR,sizeof(struct Settings));
	memcpy(geoparam,(struct GeoParam *)PARAMADDR,10*sizeof(struct GeoParam));
	if(settings.magic!=5566)
	{
		memset(&settings,0,sizeof(struct Settings));
		memset(geoparam,0,10*sizeof(struct GeoParam));
		settings.magic = 5566;
		FlashProgram();
	}
//		GUI_JPEG_Draw(_acbg2, sizeof(_acbg2), 0, 0);
		
  MX_FATFS_Init();
  //MX_USB_DEVICE_Init();
}
