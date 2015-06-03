#ifndef __AD7190_H
#define __AD7190_H
#include "stm32f4xx_hal.h"

#include "ad7190.h"

extern SPI_HandleTypeDef hspi3;

void AD7190_SPI_Write(uint8_t* data,uint8_t bytesNumber);

void AD7190_SPI_Read(uint8_t* data,uint8_t bytesNumber);

void AD7190_Reset(void);

void AD7190_Setup();
	
void AD7190_Calibration(void);

uint32_t AD7190Read(void);

void AD7190_PowerDown();

#endif
