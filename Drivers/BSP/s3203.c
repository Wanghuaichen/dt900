#include "s3203.h"
#include "ui.h"

extern I2C_HandleTypeDef hi2c3;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelvetica32;
extern struct UIInfo UIInfo;

#define I2C_ADDR 0x40
uint8_t ch[14];
uint8_t addr = 0x13;

int tpScan()
{
	if(!HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_8))
	{
		if(HAL_I2C_Master_Transmit(&hi2c3,I2C_ADDR,&addr,1,200)!=HAL_OK)
			return -1;
		
		if(HAL_I2C_Master_Receive (&hi2c3,I2C_ADDR,ch,14,200) != HAL_OK)
			return -1;
		
		if (ch[2] & 0x03)
    {
//			if(UIInfo.TouchEvent==0x3)
//				UIInfo.TouchEvent=0;
			if(UIInfo.TouchEvent<0x2)
				UIInfo.TouchEvent++;
				
       UIInfo.tpX = ((((unsigned short)ch[5]) << 4) | (ch[7] & 0x0f)) & 0x0fff;
       UIInfo.tpY = ((((unsigned short)ch[6]) << 4) | ((ch[7] & 0xf0) >> 4)) & 0x0fff;
		}
		else if(UIInfo.TouchEvent)
			UIInfo.TouchEvent=0x3;
	}
	return 0;
}