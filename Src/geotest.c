#include "geotest.h"
#include "pwr.h"
#include "dac8831.h"
#include "ad7190.h"
#include "GUI.h"
#include "flash.h"
#include <stdlib.h>
#include <math.h>
#include "ui.h"
#include "ostask.h"

extern void rdft(int n, int isgn, float *a);

uint32_t test0,test1;
uint32_t ADCMID = 0x800000;
static float Inbuff[4096];
struct Geophone geophone;
struct GeoParam geoparam[10];
struct Settings settings;
static struct GeoParam *geo;

void geotest()
{
	char str[20];
	
	geo = &geoparam[settings.paramnum];
	if(settings.sensormode == 3)
		geophone.temp = settings.temperature;
	else
		geophone.temp = geo->T;
	
	GUI_SetColor(BLACK);
	GUI_SetBkColor(WHITE);
	//GUI_SetFont(&GUI_FontHelvetica32);	
	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_TOP);
	
	AP_EN(1);
	AMP_EN(1);
	DAC_SET(DACMID);
	MUX(1);
	HAL_Delay(100);
	AD7190_Reset();
	AD7190_Calibration();
	AD7190_Setup();
	
	step0();
	sprintf(str,"Noise:%.1f",geophone.nois);
	GUI_DispStringAt(str,0,350);
	
	step1();
	sprintf(str,"R:%d",geophone.resi);
	GUI_DispStringAt(str,0,400);
	
	
	step2();
	sprintf(str,"F:%.2f    B:%.3f    S:%.1f",geophone.freq,geophone.damp,geophone.sens);
	GUI_DispStringAt(str,0,450);
	
	
	step3();
	sprintf(str,"D:%.3f  Z:%d",geophone.dist,geophone.impe);
	GUI_DispStringAt(str,0,500);
	
	AD7190_PowerDown();
	AMP_EN(0);
	AP_EN(0);
	MUX(0);
}

void step0()
{
	uint32_t i;
	double val=0;
	uint32_t max=0;
	uint32_t min=0xffffff;
	
	DAC_SET(DACMID);	
	HAL_Delay(T_DELAY);
dbg("step0 0");
	for(i=0;i<2048;i++)
	{
		Inbuff[i] = AD7190Read();
		val += Inbuff[i];
		if(Inbuff[i]>max)
			max = Inbuff[i];
		if(Inbuff[i]<min)
			min = Inbuff[i];
	}
dbg("step0 1");
	rdft(2048,1,Inbuff);
	ADCMID = Inbuff[0]/2048;
	geophone.nois	= (max-min)*5000.0/0xffffff;
}

void step1()
{
	double val=0;
	int i;
	unsigned short offset;
	float volt = geo->R*geo->X*geo->M*4*PI*PI*geo->F*geo->F/geo->S*settings.strings;
	int resi;
	int flag=0;
	
	volt = volt>7 ? 7 :	volt;
	volt = volt*R_ref/geo->R/settings.strings;
	volt = volt>2.2 ? 2.2 : volt;

	
	offset =(unsigned short)(volt/5*0xffff);
	DAC_SET(DACMID+offset);
	HAL_Delay(T_DELAY);
dbg("step1 0");
	for(i=0;i<64;i++)
		val+=AD7190Read();
dbg("step1 1");
	val /= 64;
	geophone.resi = (uint32_t)round(R_ref*AMPGAIN*abs(val-ADCMID)/(volt/5*0xffffff));
	geophone.resi /= 1+0.004*(geophone.temp-geo->T);
}

void step2()
{
	int i;
	unsigned int offset;
	unsigned int a1,a2,T,i1;
	float t;
	float volt = 0.35*0.5*geo->R*geo->X*geo->M*4*PI*PI*geo->F*geo->F/geo->S*settings.strings;
	volt = volt>7 ? 7 :	volt;
	volt = volt*R_ref/geo->R/settings.strings;
	volt = volt>2.2 ? 2.2 : volt;
	offset= volt/5*0xffff;
	i1=0;
	a1=a2=ADCMID;
	DAC_SET(DACMID+offset);
	HAL_Delay(T_DELAY);
dbg("step2 0");
	AD7190Read();
	DAC_SET(DACMID);
	for(i=0;i<4096;i++)
		Inbuff[i] = AD7190Read();
dbg("step2 1");
	for(i=5;i<4096;i++)
	{
		if(Inbuff[i]>a1)
		{
			a1=Inbuff[i];
			i1=i;
		}
		else if(Inbuff[i]<a2)
		{
			a2=Inbuff[i];
		}
	}
	i=i1;
dbg("step2 2");
	while(Inbuff[i]>=ADCMID && i<4096)
		i++;
dbg("step2 3");
	T = i-2;
	t = log((float)abs(a1-ADCMID)/abs(ADCMID-a2));
	geophone.damp = t/sqrt(PI*PI+t*t);
	t = (float)T/4096;
	geophone.freq = 1.0/(2*t*sqrt(1-geophone.damp*geophone.damp));
	t = sqrt(1-geophone.damp*geophone.damp)/geophone.damp;
	geophone.sens = sqrt(2*PI*geophone.freq*geo->M*settings.strings*abs(a1-ADCMID)*AMPGAIN/0xffffff*5.0/(volt/R_ref)*exp(atan(t)/t));
	geophone.damp /= 1-0.002*(geophone.temp-geo->T);
}
	
void step3()
{
	int i;
	int fs = 4096;
	int N = 4096;
	float mag=0.7*0.01778*geo->S*R_ref/geo->Z;
	int NF = geo->DF;
	
	DAC_SWEEP(NF,-mag,0);
  HAL_Delay(T_DELAY);
dbg("step3 0");
 	for(i=0;i<N;i++)
		Inbuff[i] = AD7190Read();
dbg("step3 1");
	DAC_SWEEP(0,0,0);
	rdft(N,1,Inbuff);
	Inbuff[1] = sqrt(Inbuff[2*NF]*Inbuff[2*NF]+Inbuff[2*NF+1]*Inbuff[2*NF+1]);
	Inbuff[2] = sqrt(Inbuff[4*NF]*Inbuff[4*NF]+Inbuff[4*NF+1]*Inbuff[4*NF+1]);
	Inbuff[3] = sqrt(Inbuff[6*NF]*Inbuff[6*NF]+Inbuff[6*NF+1]*Inbuff[6*NF+1]);
	geophone.impe = (uint32_t)round(R_ref*AMPGAIN*Inbuff[1]/(N/2)/(mag/5*0xffffff));
	geophone.dist = 100.0*(Inbuff[2]+Inbuff[3])/Inbuff[1];
}
