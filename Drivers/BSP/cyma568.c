#include "cyma568.h"

#include "ui.h"

extern I2C_HandleTypeDef hi2c3;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelvetica32;

#define I2C_ADDR 0x48
uint8_t regdata[1024];
uint16_t length;

uint8_t lauchapp[13] = {0x4,0x0,0xB,0x0,0x40,0x00,0x01,0x3B,0x00,0x00,0x20,0xC7,0x17};
uint8_t getbootinfo[13] = {0x4,0x0,0xB,0x0,0x40,0x00,0x01,0x38,0x00,0x00,0xa0,0x09,0x17};
uint8_t HidDesReg[2] = {0x1,0x0};
uint8_t SoftRst[4] = {0x05,0x00,0x00,0x01};
uint8_t Ping[7] = {0x04,0x00,0x05,0x00,0x2F,0x00,0x00};

static const uint16_t crc_table[16] = {
	0x0000, 0x1021, 0x2042, 0x3063,
	0x4084, 0x50a5, 0x60c6, 0x70e7,
	0x8108, 0x9129, 0xa14a, 0xb16b,
	0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
};

static uint16_t _cyttsp5_compute_crc(uint8_t *buf, uint32_t size)
{
	uint16_t remainder = 0xFFFF;
	uint16_t xor_mask = 0x0000;
	uint32_t index;
	uint32_t byte_value;
	uint32_t table_index;
	uint32_t crc_bit_width = sizeof(uint16_t) * 8;

	/* Divide the message by polynomial, via the table. */
	for (index = 0; index < size; index++) {
		byte_value = buf[index];
		table_index = ((byte_value >> 4) & 0x0F)
			^ (remainder >> (crc_bit_width - 4));
		remainder = crc_table[table_index] ^ (remainder << 4);
		table_index = (byte_value & 0x0F)
			^ (remainder >> (crc_bit_width - 4));
		remainder = crc_table[table_index] ^ (remainder << 4);
	}

	/* Perform the final remainder CRC. */
	return remainder ^ xor_mask;
}

void tpTest()
{
//	int i = 0;
	int ret;
	memset(regdata,0,1024);
	GUI_SetColor(WHITE);
	GUI_SetBkColor(TITLECOLOR);
	GUI_SetFont(&GUI_FontHelvetica32);	
	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_TOP);
	//GUI_DispHexAt(_cyttsp5_compute_crc(&lauchapp[6],4),0,0,4);
	//HAL_I2C_IsDeviceReady(&hi2c3,I2C_ADDR,100,200);
	//HAL_GPIO_WritePin (GPIOC,GPIO_PIN_6,GPIO_PIN_SET);
	//HAL_Delay(10);
	//ret = HAL_I2C_Master_Transmit(&hi2c3,I2C_ADDR,getbootinfo,13,1000);
	//ret = HAL_I2C_Master_Transmit(&hi2c3,I2C_ADDR,HidDesReg,2,1000);
	//ret = HAL_I2C_Master_Transmit(&hi2c3,I2C_ADDR,SoftRst,4,1000);
	//ret = HAL_I2C_Master_Transmit(&hi2c3,I2C_ADDR,lauchapp,13,200);
	//GUI_DispDecAt(ret,0,0,2);
//while(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_8)) i++;
//GUI_DispDecAt(i,0,0,2);
	//GUI_DispDecAt(HAL_I2C_IsDeviceReady(&hi2c3,0x24,100,200),300,0,2);
	GUI_DispHexAt(0xffff,200,40,4);
	GUI_DispHexAt(0xffff,200,80,4);
	if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_8))
	{
		GUI_DispHexAt(0xABCD,200,40,4);
		return;
	}
	HAL_I2C_Master_Receive (&hi2c3,I2C_ADDR,regdata,2,200);
	length = regdata[1];
	length = (length<<8)+regdata[0];
	GUI_DispHexAt(length,350,40,2);
	if(length>0)
	{
		if(HAL_I2C_Master_Receive (&hi2c3,I2C_ADDR,regdata,length,200) != HAL_OK)
			return;
	}
	GUI_DispHexAt(regdata[0],200,40,2);
	GUI_DispHexAt(regdata[1],200,80,2);
}

void HidDes()
{
	GUI_DispHexAt(0xff,0,40,2);
	GUI_DispHexAt(0xff,0,80,2);
	if(HAL_I2C_Master_Transmit(&hi2c3,I2C_ADDR,HidDesReg,2,200)!=HAL_OK)
		return;
	HAL_Delay(1000);
	if(HAL_I2C_Master_Receive (&hi2c3,I2C_ADDR,regdata,2,200) != HAL_OK)
		return;
	length = regdata[1];
	length = (length<<8)+regdata[0];
	if(length>0)
	{
		if(HAL_I2C_Master_Receive (&hi2c3,I2C_ADDR,regdata,length,1000) != HAL_OK)
			GUI_DispStringAt("fault",300,40);
	}
	GUI_DispHexAt(regdata[0],0,40,2);
	GUI_DispHexAt(regdata[1],0,80,2);
	GUI_DispHexAt(regdata[2],0,120,2);
}

void ping()
{
	memset(regdata,0xff,1024);
	if(HAL_I2C_Master_Transmit(&hi2c3,I2C_ADDR,Ping,sizeof(Ping),200)!=HAL_OK)
		return;
	while(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_8));
	if(HAL_I2C_Master_Receive (&hi2c3,I2C_ADDR,regdata,2,1000) != HAL_OK)
		GUI_DispStringAt("fault",300,40);
	length = regdata[1];
	length = (length<<8)+regdata[0];
	if(length>0)
	{
		if(HAL_I2C_Master_Receive (&hi2c3,I2C_ADDR,regdata,length,1000) != HAL_OK)
			GUI_DispStringAt("fault",300,40);
	}
	
	GUI_DispHexAt(regdata[0],0,40,2);
	GUI_DispHexAt(regdata[1],0,80,2);
	GUI_DispHexAt(regdata[2],0,120,2);
	GUI_DispHexAt(regdata[3],0,160,2);
	GUI_DispHexAt(regdata[4],0,200,2);
}

void touchreport()
{
	static int i=0;
	static int j=0;
	int n;
	if(!HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_8))
	{
		j++;
		if(HAL_I2C_Master_Receive (&hi2c3,I2C_ADDR,regdata,2,1000) != HAL_OK)
			GUI_DispStringAt("fault",300,40);
		length = regdata[1];
		length = (length<<8)+regdata[0];
		if(length>0)
		{
			if(HAL_I2C_Master_Receive (&hi2c3,I2C_ADDR,regdata,length,1000) != HAL_OK)
				GUI_DispStringAt("fault",300,40);
		}
		else
		{
			return;
		}
		i++;
		GUI_DispHexAt(regdata[0],0,120,2);
		for(n=1;n<30;n++)
		{
			GUI_DispHex(regdata[n],2);
		}
		GUI_DispDecAt(i,0,0,8);
		GUI_DispDecAt(j,240,0,8);
	}
}
