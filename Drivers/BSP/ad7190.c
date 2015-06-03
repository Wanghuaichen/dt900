#include "ad7190.h"

extern SPI_HandleTypeDef hspi3;

void AD7190_SPI_Write(uint8_t* data,uint8_t bytesNumber)
{
	HAL_SPI_Transmit(&hspi3,data,bytesNumber,1);
}

void AD7190_SPI_Read(uint8_t* data,uint8_t bytesNumber)
{
	HAL_SPI_Receive (&hspi3,data,bytesNumber,1);
} 

void AD7190_Reset(void)
{
	uint8_t dataToSend[5] = {0xff, 0xff, 0xff, 0xff, 0xff};   
	AD7190_SPI_Write(dataToSend,5);
	HAL_Delay(1);
}

void AD7190_Setup()
{
	uint8_t mode[4] = {0x08,0x04,0x00,0x01};
	uint8_t conf[4] = {0x10,0x00,0x02,0x00};
	AD7190_SPI_Write(conf,4);
	AD7190_SPI_Write(mode,4);
	HAL_Delay(200);
}

void AD7190_Calibration(void)
															//conf 3rd bytes
{                             //0x10  1-com    
															//0x02  3-4
	uint8_t conf[4] = {0x10,0x00,0x02,0x00};
	uint8_t mode[4] = {0x08,0xa4,0x02,0x80};
	uint8_t mode2[4] = {0x08,0x84,0x02,0x80};

	AD7190_SPI_Write(conf,4);
	AD7190_SPI_Write(mode2,4);
	while(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_11));
	AD7190_SPI_Write(mode,4);
	while(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_11));
}

uint32_t AD7190Read(void)
{
	uint8_t code = 0x58;
	uint32_t val = 0;
	uint8_t data[3];
	
	while(!HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_11));
	while(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_11));
	AD7190_SPI_Write(&code,1);
	AD7190_SPI_Read(data,3);
	val += data[0];
	val = (val<<8)+data[1];
	val = (val<<8)+data[2];
	return val;
}

void AD7190_PowerDown()
{
	uint8_t mode[4] = {0x68,0x04,0x00,0x01};
	while(!HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_11));
	while(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_11));
	AD7190_SPI_Write(mode,4);
}
