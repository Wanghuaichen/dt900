#include "flash.h"

void FlashProgram(uint32_t *dstAddr,uint32_t *scrAddr,uint32_t nWord)
{
	uint32_t index=0;
	
	HAL_FLASH_Unlock();
	for(index=0;index<nWord;index++)
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,(uint32_t)dstAddr+index*4,*(uint32_t *)(scrAddr+index*4));
	HAL_FLASH_Lock();
}