#include "init.h"
#include "pwr.h"
#include "adc.h"
#include "beep.h"
#include "sdram.h"
#include "lcd.h"
#include "spiflash.h"
#include "GUI.h"
#include "ui.h"
#include "geotest.h"
#include "flash.h"

extern struct GeoParam geoparam[10];//(struct GeoParam *)PARAMADDR;
extern struct Settings settings;//(struct Settings *)SETADDR;

static void FlashReadProtection()
{	
	FLASH_OBProgramInitTypeDef OBInit; 
	
	HAL_FLASHEx_OBGetConfig(&OBInit);
	if (OBInit.RDPLevel==RESET )
	{
		OBInit.OptionType = OPTIONBYTE_RDP;
		OBInit.RDPLevel=OB_RDP_LEVEL_1 ;
		
		HAL_FLASH_OB_Unlock();
		HAL_FLASH_Unlock();
		HAL_FLASHEx_OBProgram(&OBInit); 
		HAL_FLASH_OB_Launch();
		HAL_FLASH_OB_Lock();
		HAL_FLASH_Lock();
	}
}

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
	memcpy(&settings,(struct Settings *)SETADDR,sizeof(struct Settings));
	memcpy(geoparam,(struct GeoParam *)PARAMADDR,10*sizeof(struct GeoParam));
	if(settings.magic!=5566)
	{
		memset(&settings,0,sizeof(struct Settings));
		memset(geoparam,0,10*sizeof(struct GeoParam));
		settings.magic = 5566;
		settings.backlight = 20;
		settings.buzzer = 1;
		settings.iteration = 1;
		settings.serialno = 1;
		settings.series = 1;
		settings.parallel = 1;
		settings.constant=1; //1 velocity 0 excurtion
		settings.sleeptime = 5;
		settings.shuttime = 20;
		settings.temperature = 25;
		sprintf(settings.filename,"DTCCSOLO");
		FlashProgram();
	}
	
  MX_FATFS_Init();
	HAL_Delay(200);
	
	LCD_PWR(1);
	beepoff();
	getuid();
//	FlashReadProtection();
}
