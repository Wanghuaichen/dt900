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
#include "ostask.h"

extern struct GeoParam geoparam[10];//(struct GeoParam *)PARAMADDR;
extern struct Settings settings;//(struct Settings *)SETADDR;
extern IWDG_HandleTypeDef IwdgHandle;

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
	int param[] = 
	{
		0x532d5444,0x314f4c4f,0x005a4830,0x00000000,0x00000000,0x44e10000,0x3d0f5c29,0x3d0f5c29,
		0x3f028f5c,0x3d75c28f,0x3d75c28f,0x42ab999a,0x3d0f5c29,0x3d0f5c29,0x41200000,0x3d0f5c29,
		0x3d0f5c29,0x46136c00,0x00000000,0x00000000,0x3dcccccd,0x3c656042,0x3b656042,0x41c80000,
		0x41400000,0x532d5444,0x354f4c4f,0x00005a48,0x00000000,0x00000000,0x44e74000,0x3d4ccccd,
		0x3d4ccccd,0x3f19999a,0x3d99999a,0x3d99999a,0x42a00000,0x3d4ccccd,0x3d4ccccd,0x40a00000,
		0x3d99999a,0x3d99999a,0x459ed000,0x00000000,0x00000000,0x3dcccccd,0x3cb9f55a,0x3b449ba6,
		0x41b00000,0x41400000
	};
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
	AP_EN(1);
	beepon();
	//FlashReadProtection();
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
		settings.constant=1; //1 velocity 0 excursion
		settings.sleeptime = 5;
		settings.shuttime = 20;
		settings.temperature = 25;
		sprintf(settings.filename,"DTCCSOLO");
		settings.totalparam = 2;
		memcpy(geoparam,param,200);
		FlashProgram();
	}
	
  MX_FATFS_Init();
	HAL_Delay(200);
	
	LCD_PWR(1);
	beepoff();
	getuid();
}
