#ifndef __DS18B20_H
#define __DS18B20_H
#include "stm32f4xx_hal.h"

void DS18B20_IO_IN(void);
void DS18B20_IO_OUT(void);										   
unsigned char DS18B20_DQ_IN(void);
void DS18B20_DQ_OUT(int code);
   	
unsigned char DS18B20_Init(void);//��ʼ��DS18B20
float DS18B20_Get_Temp(void);//��ȡ�¶�
void DS18B20_Start(void);//��ʼ�¶�ת��
void DS18B20_Write_Byte(unsigned char dat);//д��һ���ֽ�
unsigned char DS18B20_Read_Byte(void);//����һ���ֽ�
unsigned char DS18B20_Read_Bit(void);//����һ��λ
unsigned char DS18B20_Check(void);//����Ƿ����DS18B20
void DS18B20_Rst(void);//��λDS18B20    
#endif
