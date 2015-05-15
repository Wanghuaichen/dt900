#include "geotest.h"
#include "pwr.h"
#include "dac8831.h"
#include "ad7190.h"
#include "GUI.h"
#include "flash.h"
#include <stdlib.h>

static float Inbuff[4096];
uint32_t test0,test1;

void geotest()
{
	int i;
	uint32_t color=0;
	char string[24];
	
	AP_EN(1);
	AMP_EN(1);
	MUX(0);
	//DAC_SET(0x7fff);
	DAC_SET(0x7fff+0xffff/5);
	osDelay(100);
//	AD7190_Reset();
	AD7190_Calibration();
	AD7190_Setup();
	//osDelay(200);
	//GUI_JPEG_Draw(_acbg2, sizeof(_acbg2), 0, 0);
	while(1)
	{
		if(color == 0)
			color = 0x001e29ce;
		else
			color = 0;
		GUI_SetColor(color);
		for(i=0;i<30;i++)
		{
			//sprintf(string," %04x ",AD7190Read());
			//GUI_DispStringAt(string,200,i*32);
			GUI_DispHexAt(AD7190Read(),0,i*48,4);
			//GUI_DispCEOL();
		}
		osDelay(1000);
	}

//	DAC_SWEEP(10,1,0);
}

void geotest2()
{
	test1 = 0x11220044;
	test0 = *(uint32_t *)PARAMADDR;
	GUI_DispHexAt(test0,0,700,8);
	FlashProgram((uint32_t *)PARAMADDR,&test1,1);
	test0 = *(uint32_t *)PARAMADDR;
	GUI_DispHexAt(test0,240,700,8);
}

void geotest3()
{
	AP_EN(1);
	AMP_EN(1);
	osDelay(1000);
	MUX(0);
	//GUI_DispStringAt("key1",240,600);
	AD7190_Reset();
	AD7190_Calibration();
	AD7190_Setup();
	//GUI_DispStringAt("key2",240,600);
	
while(1)
{
//	step0();
	step1();
	osDelay(3000);
}
	
	
	
//	AD7190_PowerDown();
//	AP_EN(0);
//	AMP_EN(0);
}


void step0()
{
	int i;
	double val=0;
	float noise;
	char string[20];
	MUX(0);
	DAC_SET(DACMID);	
	osDelay(T_DELAY);
	GUI_DispStringAt("A           ",0,600);
	for(i=0;i<2048;i++)
		val += abs(AD7190Read()-ADCMID);
//	for(i=0;i<30;i++)
//	{
//		GUI_DispHexAt(AD7190Read(),240,i*48,4);
//	}
	noise = 4*val/2048*5000/0xffff;
	sprintf(string,"Noise:%f",noise);
	GUI_DispStringAt(string,0,600);
}

void step1()
{
	double val=0;
	int i;
	unsigned short offset;
//	struct geoparameter *param = &geoset.geoparam[geoset.paramnum];
//	float volt = param->R*param->X*param->M*4*PI*PI*param->F*param->F/param->S;
	float volt = 1;
	int resi;
	char string[40];
//	volt = volt>2 ? 2 : volt;
//	volt = volt*R_ref/param->R;
//	volt = volt>2 ? 2 : volt;

	offset =(unsigned short)(volt/5*0xffff);
	MUX(1);
	DAC_SET(DACMID+offset);
	osDelay(T_DELAY);
	for(i=0;i<64;i++)
		val+=AD7190Read();	
	val /= 64;
	GUI_DispDecAt((int)abs(val-ADCMID),0,600,6);
	resi = (int)(R_ref*4*abs(val-ADCMID)/(volt/5*0xffff));
//	geophone.resi = R_ref*abs(val-MIDV)/(volt/5*0xffff);
//	geophone.resi /= 1+0.004*(geophone.temp-param->T);
sprintf(string,"Resistence:%d",resi);
	GUI_DispStringAt(string,0,700);
}