#include "ds18b20.h"
//PD12
//PB1
volatile char dsflag;
void DS18B20_IO_IN()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	if(!dsflag)
	{		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	     
		GPIO_Init(GPIOC, &GPIO_InitStructure);
	}
	else
	{
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	     
		GPIO_Init(GPIOC, &GPIO_InitStructure);
	}
}

void DS18B20_IO_OUT()
{
	GPIO_InitTypeDef  GPIO_InitStructure; 
	if(!dsflag)
	{
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;;	     
		GPIO_Init(GPIOC, &GPIO_InitStructure);
	}
	else
	{
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;;	     
		GPIO_Init(GPIOC, &GPIO_InitStructure);
	}
}

void DS18B20_DQ_OUT(int code)
{
	if(!dsflag)
	{
		if(code)
			GPIO_SetBits(GPIOC,GPIO_Pin_8);
		else
			GPIO_ResetBits(GPIOC,GPIO_Pin_8);
	}	
	else
	{
		if(code)
			GPIO_SetBits(GPIOC,GPIO_Pin_12);
		else
			GPIO_ResetBits(GPIOC,GPIO_Pin_12);
	}
}

unsigned char DS18B20_DQ_IN()
{
	if(!dsflag)
		return GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8);
	else
		return GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12);
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
//等待DS18B20的回应
//返回1:未检测到DS18B20的存在
//返回0:存在
unsigned char DS18B20_Check(void) 	   
{   
	u8 retry=0;
	DS18B20_IO_IN();//SET PA0 INPUT	 
    while (DS18B20_DQ_IN()&&retry<200)
	{
		retry++;
		delay_us(1);
	};
	//USART_OUT(USART1,"001");	
	if(retry>=200)
		return 1;
		else retry=0;
		//USART_OUT(USART1,"002");	
    while (!DS18B20_DQ_IN()&&retry<240)
	{
		retry++;
		delay_us(1);
	};
	if(retry>=240)
		return 1;	
//USART_OUT(USART1,"003");	
	return 0;
}
//从DS18B20读取一个位
//返回值：1/0
unsigned char DS18B20_Read_Bit(void) 			 // read one bit
{
    u8 data;
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
    u8 i,j,dat;
    dat=0;
	for (i=1;i<=8;i++) 
	{
        j=DS18B20_Read_Bit();
        dat=(j<<7)|(dat>>1);
    }						    
    return dat;
}
//写一个字节到DS18B20
//dat：要写入的字节
void DS18B20_Write_Byte(unsigned char dat)     
 {             
    u8 j;
    u8 testb;
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
//开始温度转换
void DS18B20_Start(void)// ds1820 start convert
{   						               
    DS18B20_Rst();	   
	DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);// skip rom
    DS18B20_Write_Byte(0x44);// convert
} 
//初始化DS18B20的IO口 DQ 同时检测DS的存在
//返回1:不存在
//返回0:存在    	 
unsigned char DS18B20_Init(void)
{
 	GPIO_InitTypeDef  GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;	
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		  
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOC, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOC,GPIO_Pin_12);
	dsflag=1;
	DS18B20_Rst();
	DS18B20_Get_Temp();
	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		  
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOC, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOC,GPIO_Pin_8);
	dsflag=0;
	DS18B20_Rst();
	DS18B20_Get_Temp();
}  
//从ds18b20得到温度值
//精度：0.1C
//返回值：温度值 （-550~1250） 
float DS18B20_Get_Temp(void)
{
    u8 temp;
    u8 TL,TH;
		short tem;
    DS18B20_Start ();                    // ds1820 start convert
    DS18B20_Rst();
    DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);// skip rom
    DS18B20_Write_Byte(0xbe);// convert	    
    TL=DS18B20_Read_Byte(); // LSB   
    TH=DS18B20_Read_Byte(); // MSB  
	  //USART_OUT(USART1,"TEMP:%x  %x\r\n",TH,TL);  	  
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
//     tem=(float)tem*0.625;//转换  
// 	if(temp)return tem; //返回温度值
// 	else return -tem;    
} 
