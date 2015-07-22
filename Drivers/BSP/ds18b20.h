#ifndef __DS18B20_H
#define __DS18B20_H
#include "stm32f4xx_hal.h"

void DS18B20_IO_IN(void);
void DS18B20_IO_OUT(void);										   
unsigned char DS18B20_DQ_IN(void);
void DS18B20_DQ_OUT(int code);
   	
unsigned char DS18B20_Init(void);//初始化DS18B20
float DS18B20_Get_Temp(void);//获取温度
void DS18B20_Start(void);//开始温度转换
void DS18B20_Write_Byte(unsigned char dat);//写入一个字节
unsigned char DS18B20_Read_Byte(void);//读出一个字节
unsigned char DS18B20_Read_Bit(void);//读出一个位
unsigned char DS18B20_Check(void);//检测是否存在DS18B20
void DS18B20_Rst(void);//复位DS18B20    
#endif
