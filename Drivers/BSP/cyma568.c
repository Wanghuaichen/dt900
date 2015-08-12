#include "cyma568.h"

#include "ui.h"

extern I2C_HandleTypeDef hi2c3;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelvetica32;

uint8_t regdata[1024];

uint8_t lauchapp[13] = {0x4,0x0,0xB,0x0,0x40,0x00,0x01,0x3b,0x00,0x00,0x00,0x00,0x17};
uint8_t getbootinfo[13] = {0x4,0x0,0xB,0x0,0x40,0x00,0x01,0x38,0x00,0x00,0xa0,0x09,0x17};
void tpTest()
{
	int i = 0;
	int ret;
	memset(regdata,0,1024);
	GUI_SetColor(WHITE);
	GUI_SetBkColor(TITLECOLOR);
	GUI_SetFont(&GUI_FontHelvetica32);	
	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_TOP);
	HAL_GPIO_WritePin (GPIOC,GPIO_PIN_6,GPIO_PIN_SET);
	HAL_Delay(10);
	//ret = HAL_I2C_Master_Transmit(&hi2c3,0x24,getbootinfo,13,1000);
	//GUI_DispDecAt(ret,0,0,2);
//	//while(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_8)) i++;
//	//GUI_DispDecAt(i,0,0,2);
	GUI_DispDecAt(HAL_I2C_IsDeviceReady(&hi2c3,0x24,100,200),0,0,2);
	ret = HAL_I2C_Master_Receive (&hi2c3,0x24,regdata,2,200);
	GUI_DispDecAt(ret,300,0,2);
	GUI_DispDecAt(regdata[0],0,40,4);
	GUI_DispDecAt(regdata[1],0,80,4);
}


