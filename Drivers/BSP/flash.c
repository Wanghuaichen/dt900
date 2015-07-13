#include "flash.h"
#include "geotest.h"
extern struct GeoParam geoparam[10];//(struct GeoParam *)PARAMADDR;
extern struct Settings settings;//(struct Settings *)SETADDR;

void FlashProgram()
{
	uint32_t index;
	uint32_t error;
	FLASH_EraseInitTypeDef erase;
	
	erase.TypeErase = TYPEERASE_SECTORS;
	erase.VoltageRange = VOLTAGE_RANGE_3;
	erase.Banks = FLASH_BANK_2;
	erase.NbSectors = 1;
	erase.Sector = FLASH_SECTOR_23;
	HAL_FLASH_Unlock();
	HAL_FLASHEx_Erase(&erase,&error);
	
	
	for(index=0;index<10*sizeof(struct GeoParam);index+=4)
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,PARAMADDR+index,*((int*)((int)geoparam+index)));	
	for(index=0;index<sizeof(struct Settings);index+=4)
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,SETADDR+index,*((int*)((int)&settings+index)));
	HAL_FLASH_Lock();
}