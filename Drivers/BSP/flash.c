#include "flash.h"
#include "geotest.h"
extern struct GeoParam geoparam[10];
extern struct Settings settings;

void FlashProgram()
{
	uint32_t index;
	uint32_t error;
	uint32_t ret;
	FLASH_EraseInitTypeDef erase;
	
	erase.TypeErase = TYPEERASE_SECTORS;
	erase.VoltageRange = VOLTAGE_RANGE_3;
	erase.Banks = FLASH_BANK_2;
	erase.NbSectors = 1;
	erase.Sector = FLASH_SECTOR_23;
	ret = HAL_FLASH_Unlock();
	ret = HAL_FLASHEx_Erase(&erase,&error);
	
	
	for(index=0;index<10*sizeof(struct GeoParam);index+=4)
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,PARAMADDR+index,*((int*)((int)geoparam+index)));	
	for(index=0;index<sizeof(struct Settings);index+=4)
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,SETADDR+index,*((int*)((int)&settings+index)));
	HAL_FLASH_Lock();
}

void FlashPasswd(long num)
{
	uint32_t error;
	FLASH_EraseInitTypeDef erase;
	
	erase.TypeErase = TYPEERASE_SECTORS;
	erase.VoltageRange = VOLTAGE_RANGE_3;
	erase.Banks = FLASH_BANK_2;
	erase.NbSectors = 1;
	erase.Sector = FLASH_SECTOR_22;
	HAL_FLASH_Unlock();
	HAL_FLASHEx_Erase(&erase,&error);
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,PASSADDR,num);	
	HAL_FLASH_Lock();
}

void FlashEraseTotal24()
{
	FLASH_EraseInitTypeDef erase;
	uint32_t error;
	
	erase.TypeErase = TYPEERASE_MASSERASE;
	erase.VoltageRange = VOLTAGE_RANGE_3;
	erase.Banks = FLASH_BANK_BOTH;
	erase.NbSectors = 24;
	erase.Sector = FLASH_SECTOR_TOTAL;
	HAL_FLASH_Unlock();
	HAL_FLASHEx_Erase(&erase,&error);
	HAL_FLASH_Lock();
}