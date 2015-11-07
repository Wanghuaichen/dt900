#include "ds18b20.h"
//PA5
//PA14
#include "geotest.h"
extern struct Settings settings;

static void delay_us(int code)
{
	volatile int i;
	for(i=0;i<code*10;i++);
}

void DS18B20_IO_IN()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.Pin = GPIO_PIN_14;//settings.sensormode==1 ? GPIO_PIN_5 : GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void DS18B20_IO_OUT()
{
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Pin = GPIO_PIN_14;//settings.sensormode==1 ? GPIO_PIN_5 : GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void DS18B20_DQ_OUT(int code)
{
	uint16_t pin = GPIO_PIN_14;//settings.sensormode==1 ? GPIO_PIN_5 : GPIO_PIN_14;
	HAL_GPIO_WritePin (GPIOA,pin,code);
}

unsigned char DS18B20_DQ_IN()
{
	uint16_t pin = GPIO_PIN_14;//settings.sensormode==1 ? GPIO_PIN_5 : GPIO_PIN_14;
	return HAL_GPIO_ReadPin(GPIOA,pin);
}

//复位DS18B20
void DS18B20_Rst(void)	   
{                 
	DS18B20_IO_OUT(); //SET PA0 OUTPUT
  DS18B20_DQ_OUT(0); //拉低DQ
  delay_us(750);    //拉低750us
  DS18B20_DQ_OUT(1); //DQ=1 
	delay_us(15);     //15US
}

unsigned char DS18B20_Check(void) 	   
{   
	uint8_t retry=0;
	DS18B20_IO_IN();
  while (DS18B20_DQ_IN()&&retry<200)
	{
		retry++;
		delay_us(1);
	};
	if(retry>=200)
		return 1;
	else
		retry=0;
  while (!DS18B20_DQ_IN()&&retry<240)
	{
		retry++;
		delay_us(1);
	};
	if(retry>=240)
		return 1;	
	
	return 0;
}

unsigned char DS18B20_Read_Bit(void) 			 // read one bit
{
    uint8_t data;
	DS18B20_IO_OUT();//SET PA0 OUTPUT
    DS18B20_DQ_OUT(0); 
	delay_us(2);
    DS18B20_DQ_OUT(1); 
	DS18B20_IO_IN();//SET PA0 INPUT
	delay_us(12);
	if(DS18B20_DQ_IN())data=1;
    else data=0;	 
    delay_us(50);           
    return data;
}
//从DS18B20读取一个字节
//返回值：读到的数据
unsigned char DS18B20_Read_Byte(void)    // read one byte
{        
    uint8_t i,j,dat;
    dat=0;
	for (i=1;i<=8;i++) 
	{
        j=DS18B20_Read_Bit();
        dat=(j<<7)|(dat>>1);
    }						    
    return dat;
}

void DS18B20_Write_Byte(unsigned char dat)     
 {             
    uint8_t j;
    uint8_t testb;
	DS18B20_IO_OUT();//SET PA0 OUTPUT;
    for (j=1;j<=8;j++) 
	{
        testb=dat&0x01;
        dat=dat>>1;
        if (testb) 
        {
            DS18B20_DQ_OUT(0);// Write 1
            delay_us(2);                            
            DS18B20_DQ_OUT(1);
            delay_us(60);             
        }
        else 
        {
            DS18B20_DQ_OUT(0);// Write 0
            delay_us(60);             
            DS18B20_DQ_OUT(1);
            delay_us(2);                          
        }
    }
}

void DS18B20_Start(void)// ds1820 start convert
{   						               
    DS18B20_Rst();	   
		DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);// skip rom
    DS18B20_Write_Byte(0x44);// convert
}  

float DS18B20_Get_Temp(void)
{
    uint8_t temp;
    uint8_t TL,TH;
		short tem;
		
		DS18B20_Start();
    DS18B20_Rst();
    if(DS18B20_Check())
			return 0;
    DS18B20_Write_Byte(0xcc);// skip rom
    DS18B20_Write_Byte(0xbe);// convert	    
    TL=DS18B20_Read_Byte(); // LSB   
    TH=DS18B20_Read_Byte(); // MSB  	  
    if(TH>7)
    {
        TH=~TH;
        TL=~TL; 
        temp=0;//温度为负  
    }else temp=1;//温度为正	  	  
    tem=TH; //获得高八位
    tem<<=8;    
    tem+=TL;//获得底八位
		tem = temp ? tem : -tem;
		return 0.0625*tem;   
} 
