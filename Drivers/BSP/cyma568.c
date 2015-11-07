#include "cyma568.h"
#include "ui.h"

extern I2C_HandleTypeDef hi2c3;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelvetica32;
extern struct UIInfo UIInfo;
#define I2C_ADDR 0x48
uint8_t regdata[1024];

//uint8_t lauchapp[13] = {0x4,0x0,0xB,0x0,0x40,0x00,0x01,0x3B,0x00,0x00,0x20,0xC7,0x17};
//uint8_t getbootinfo[13] = {0x4,0x0,0xB,0x0,0x40,0x00,0x01,0x38,0x00,0x00,0xa0,0x09,0x17};
//uint8_t HidDesReg[2] = {0x1,0x0};
////uint8_t SoftRst[4] = {0x05,0x00,0x00,0x01};
//uint8_t Ping[7] = {0x04,0x00,0x05,0x00,0x2F,0x00,0x00};
//uint8_t SysInfo[7] = {0x04,0x00,0x05,0x00,0x2f,0x00,0x02};
//uint8_t suspend[7] = {0x4,0x0,0x5,0x0,0x2f,0x0,0x3};
//uint8_t resume[7] = {0x4,0x0,0x5,0x0,0x2f,0x0,0x4};
//uint8_t calibration[8] = {0x4,0x0,0x6,0x0,0x2f,0x0,0x28,0x0};
//uint8_t shorttest[9] = {0x4,0x0,0x7,0x0,0x2f,0x0,0x26,0x2,0x0};
//uint8_t opentest[9] = {0x4,0x0,0x7,0x0,0x2f,0x0,0x26,0x3,0x0};

static int sendReport(uint8_t* report,int size)
{
	if(HAL_I2C_Master_Transmit(&hi2c3,I2C_ADDR,report,size,200)!=HAL_OK)
		return -1;
	return 0;
}

static int getReport()
{
	int length;
	
	if(HAL_I2C_Master_Receive (&hi2c3,I2C_ADDR,regdata,2,200) != HAL_OK)
		return -2;
	length = regdata[1];
	length = (length<<8)+regdata[0];
	if(length>2)
	{
		if(HAL_I2C_Master_Receive (&hi2c3,I2C_ADDR,regdata,length,200) != HAL_OK)
			return -3;
	}
	return length;
}

//int OpenTest()
//{
//	sendReport(opentest,9);
//	while(1)
//	{
//		if(getReport()>2 && regdata[2]==0x1F)
//			return 0;
//	}
//}

//int ShortTest()
//{
//	sendReport(shorttest,9);
//	while(1)
//	{
//		if(getReport()>2 && regdata[2]==0x1F)
//			return 0;
//	}
//}

//int CalibrateIDAC()
//{
//	int flag=1;
//	sendReport(calibration,8);
//	while(1)
//	{
//		if(getReport()>2 && regdata[2]==0x1F && (regdata[4]&0xf)==0x8)
//			return regdata[5];
//	}
//}

//int SuspendScan()
//{
//	int ret;
//	if(sendReport(suspend,7)!=0)
//		return -1;
//	while(1)
//	{
//		if(getReport()>2 && regdata[2]==0x1F && (regdata[4]&0xf)==0x3)
//			return 0;
//	}
//}

//int ResumeScan()
//{
//	int ret;
//	if(sendReport(resume,7)!=0)
//		return -1;
//	while(1)
//	{
//		if(getReport()>2 && regdata[2]==0x1F && (regdata[4]&0xf)==0x4)
//			return 0;
//	}
//}

//void analysis(int length)
//{
//	static int num=0;
//	int nReport;
//	int index;
//	int temp;
//	char str[20];
//	uint8_t* p;
//	nReport = (length-7)/10;
//	GUI_DispDecAt(num++,0,400,6);
//	for(index=0;index<nReport;index++)
//	{
//		p = &regdata[7+index*10];
//		sprintf(str,"Type:%d",p[0]&0x7);
//		GUI_DispStringAtCEOL(str,index*200,0);
//		sprintf(str,"Tip:%d",p[1]>>7);
//		GUI_DispStringAtCEOL(str,index*200,40);
//		sprintf(str,"Event ID:%d",(p[1]>>5)&0x3);
//		GUI_DispStringAtCEOL(str,index*200,80);
//		sprintf(str,"Touch ID:%d",p[1]&0x1F);
//		GUI_DispStringAtCEOL(str,index*200,120);
//		temp=p[3];
//		temp = (temp<<8)+p[2];
//		sprintf(str,"X:%d",temp);
//		GUI_DispStringAtCEOL(str,index*200,160);
//		temp=p[5];
//		temp = (temp<<8)+p[4];
//		sprintf(str,"Y:%d",temp);
//		GUI_DispStringAtCEOL(str,index*200,200);
//		sprintf(str,"Pressure:%d",p[6]);
//		GUI_DispStringAtCEOL(str,index*200,240);
//		sprintf(str,"Major:%d",p[7]);
//		GUI_DispStringAtCEOL(str,index*200,280);
//		sprintf(str,"Minor:%d",p[8]);
//		GUI_DispStringAtCEOL(str,index*200,320);
//		sprintf(str,"Oritation:%d",p[9]);
//		GUI_DispStringAtCEOL(str,index*200,360);
//	}
//}

void parseReport()
{
	uint16_t temp;
	
	//Large Object detected
	if((regdata[5]>>5)&0x1)
		return;
	
	//Parse Event,X,Y
	UIInfo.TouchEvent = (regdata[8]>>5)&0x3;
	temp=regdata[10];
	temp = (temp<<8)+regdata[9];
	UIInfo.tpY = 799-temp;
	temp=regdata[12];
	temp = (temp<<8)+regdata[11];
	UIInfo.tpX = 479-temp;
//GUI_DrawPoint(UIInfo.tpX,UIInfo.tpY);
}
	
int tpScan()
{
	int length=0;
	if(!HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_8))
	{
		length = getReport();
		if(regdata[2]==0x1)
			//analysis(length);
			parseReport();
	}
	return length;
}
	
//static const uint16_t crc_table[16] = {
//	0x0000, 0x1021, 0x2042, 0x3063,
//	0x4084, 0x50a5, 0x60c6, 0x70e7,
//	0x8108, 0x9129, 0xa14a, 0xb16b,
//	0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
//};

//static uint16_t _cyttsp5_compute_crc(uint8_t *buf, uint32_t size)
//{
//	uint16_t remainder = 0xFFFF;
//	uint16_t xor_mask = 0x0000;
//	uint32_t index;
//	uint32_t byte_value;
//	uint32_t table_index;
//	uint32_t crc_bit_width = sizeof(uint16_t) * 8;

//	/* Divide the message by polynomial, via the table. */
//	for (index = 0; index < size; index++) {
//		byte_value = buf[index];
//		table_index = ((byte_value >> 4) & 0x0F)
//			^ (remainder >> (crc_bit_width - 4));
//		remainder = crc_table[table_index] ^ (remainder << 4);
//		table_index = (byte_value & 0x0F)
//			^ (remainder >> (crc_bit_width - 4));
//		remainder = crc_table[table_index] ^ (remainder << 4);
//	}

//	/* Perform the final remainder CRC. */
//	return remainder ^ xor_mask;
//}

//void tpTest()
//{
////	int i = 0;
//	int ret;
//	memset(regdata,0,1024);
//	GUI_SetColor(WHITE);
//	GUI_SetBkColor(TITLECOLOR);
//	GUI_SetFont(&GUI_FontHelvetica32);	
//	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_TOP);
//	//GUI_DispHexAt(_cyttsp5_compute_crc(&lauchapp[6],4),0,0,4);
//	//HAL_I2C_IsDeviceReady(&hi2c3,I2C_ADDR,100,200);
//	//HAL_GPIO_WritePin (GPIOC,GPIO_PIN_6,GPIO_PIN_SET);
//	//HAL_Delay(10);
//	//ret = HAL_I2C_Master_Transmit(&hi2c3,I2C_ADDR,getbootinfo,13,1000);
//	//ret = HAL_I2C_Master_Transmit(&hi2c3,I2C_ADDR,HidDesReg,2,1000);
//	//ret = HAL_I2C_Master_Transmit(&hi2c3,I2C_ADDR,SoftRst,4,1000);
//	//ret = HAL_I2C_Master_Transmit(&hi2c3,I2C_ADDR,lauchapp,13,200);
//	//GUI_DispDecAt(ret,0,0,2);
////while(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_8)) i++;
////GUI_DispDecAt(i,0,0,2);
//	//GUI_DispDecAt(HAL_I2C_IsDeviceReady(&hi2c3,0x24,100,200),300,0,2);
//	GUI_DispHexAt(0xffff,200,40,4);
//	GUI_DispHexAt(0xffff,200,80,4);
//	if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_8))
//	{
//		GUI_DispHexAt(0xABCD,200,40,4);
//		return;
//	}
//	HAL_I2C_Master_Receive (&hi2c3,I2C_ADDR,regdata,2,200);
//	length = regdata[1];
//	length = (length<<8)+regdata[0];
//	GUI_DispHexAt(length,350,40,2);
//	if(length>0)
//	{
//		if(HAL_I2C_Master_Receive (&hi2c3,I2C_ADDR,regdata,length,200) != HAL_OK)
//			return;
//	}
//	GUI_DispHexAt(regdata[0],200,40,2);
//	GUI_DispHexAt(regdata[1],200,80,2);
//}

//void HidDes()
//{
//	sendReport(HidDesReg,2);
//	while(1)
//	{
//		if(getReport()>2 && regdata[0]==0x20)
//		{
//			GUI_DispHexAt(regdata[0],240,40,2);
//			GUI_DispHexAt(regdata[1],240,80,2);
//			GUI_DispHexAt(regdata[2],240,120,2);
//			return;
//		}
//	}
//}

//void HidDes()
//{
//	if(HAL_I2C_Master_Transmit(&hi2c3,I2C_ADDR,HidDesReg,2,200)!=HAL_OK)
//		return;
//	HAL_Delay(1000);
//	if(HAL_I2C_Master_Receive (&hi2c3,I2C_ADDR,regdata,2,200) != HAL_OK)
//		return;
//	length = regdata[1];
//	length = (length<<8)+regdata[0];
//	if(length>2)
//	{
//		if(HAL_I2C_Master_Receive (&hi2c3,I2C_ADDR,regdata,length,1000) != HAL_OK)
//			GUI_DispStringAt("fault",300,40);
//	}
//	GUI_DispHexAt(regdata[0],0,40,2);
//	GUI_DispHexAt(regdata[1],0,80,2);
//	GUI_DispHexAt(regdata[2],0,120,2);
//}

//void ping()
//{
//	memset(regdata,0xff,1024);
//	if(HAL_I2C_Master_Transmit(&hi2c3,I2C_ADDR,Ping,sizeof(Ping),200)!=HAL_OK)
//		return;
//	while(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_8));
//	if(HAL_I2C_Master_Receive (&hi2c3,I2C_ADDR,regdata,2,1000) != HAL_OK)
//		GUI_DispStringAt("fault",300,40);
//	length = regdata[1];
//	length = (length<<8)+regdata[0];
//	if(length>0)
//	{
//		if(HAL_I2C_Master_Receive (&hi2c3,I2C_ADDR,regdata,length,1000) != HAL_OK)
//			GUI_DispStringAt("fault",300,40);
//	}
//	
//	GUI_DispHexAt(regdata[0],0,40,2);
//	GUI_DispHexAt(regdata[1],0,80,2);
//	GUI_DispHexAt(regdata[2],0,120,2);
//	GUI_DispHexAt(regdata[3],0,160,2);
//	GUI_DispHexAt(regdata[4],0,200,2);
//}

//void touchreport()
//{
//	static int i=0;
//	static int j=0;
//	int n;
//	if(!HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_8))
//	{
//		j++;
//		if(HAL_I2C_Master_Receive (&hi2c3,I2C_ADDR,regdata,2,1000) != HAL_OK)
//			GUI_DispStringAt("fault1",300,40);
//		length = regdata[1];
//		length = (length<<8)+regdata[0];
//		if(length>2)
//		{
//			if(HAL_I2C_Master_Receive (&hi2c3,I2C_ADDR,regdata,length,1000) != HAL_OK)
//				GUI_DispStringAt("fault2",300,40);
//		}
//		else
//		{
//			return;
//		}
//		i++;
//		GUI_DispHexAt(regdata[7],0,120,2);
//		for(n=8;n<16;n++)
//		{
//			GUI_DispString(" ");
//			GUI_DispHex(regdata[n],2);			
//		}
//		GUI_DispCEOL();
//		GUI_DispDecAt(i,0,0,8);
//		GUI_DispDecAt(j,240,0,8);
//	}
//}

//void sysinfo()
//{
//	memset(regdata,0xff,1024);
//	if(HAL_I2C_Master_Transmit(&hi2c3,I2C_ADDR,SysInfo,sizeof(SysInfo),200)!=HAL_OK)
//	{
//		GUI_DispStringAt("fault",300,40);
//		return;
//	}
//	while(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_8));
//	if(HAL_I2C_Master_Receive (&hi2c3,I2C_ADDR,regdata,2,1000) != HAL_OK)
//	{
//		GUI_DispStringAt("fault",300,40);
//		return;
//	}
//	
//	length = regdata[1];
//	length = (length<<8)+regdata[0];
//	if(length>0)
//	{
//		if(HAL_I2C_Master_Receive (&hi2c3,I2C_ADDR,regdata,length,1000) != HAL_OK)
//			GUI_DispStringAt("fault",300,40);
//	}
//	
//	if(regdata[2]==0x1f)
//		GUI_DispHexAt(regdata[0],0,40,2);
//}
