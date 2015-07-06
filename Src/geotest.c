#include "geotest.h"
#include "pwr.h"
#include "dac8831.h"
#include "ad7190.h"
#include "GUI.h"
#include "flash.h"
#include <stdlib.h>
#include <math.h>

static float Inbuff[4096];
uint32_t test0,test1;
uint32_t ADCMID = 0x800000;
struct Geophone geophone;
struct GeoParam *geoparam = (struct GeoParam *)PARAMADDR;
struct Settings *settings = (struct Settings *)SETADDR;

extern void rdft(int n, int isgn, float *a);

/*
struct GeoParam geo =
{
	"SOLO5",//	char type[20];
	1850,//float R;
	0,//float Rp;
	0,//float Rn;
	0.6,//float B;
	0,//float Bp;
	0,//float Bn;
	80,//float S;
	0,//float Sp;
	0,//float Sn;
	5,//float F;
	0,//float Fp;
	0,//float Fn;
	5084,//float Z;
	0,//float Zp;
	0,//float Zn;
	0.07,//float D;
	22.7,//float M;
	3,//float X;
	22,//float T;
	12,//float DF;
};*/


struct GeoParam geo =
{
	"SOLO15",//	char type[20];
	1564,//float R;
	0,//float Rp;
	0,//float Rn;
	0.6,//float B;
	0,//float Bp;
	0,//float Bn;
	74.8,//float S;
	0,//float Sp;
	0,//float Sn;
	15,//float F;
	0,//float Fp;
	0,//float Fn;
	5312,//float Z;
	0,//float Zp;
	0,//float Zn;
	0.17,//float D;
	13.2,//float M;
	4.06,//float X;
	22,//float T;
	15,//float DF;
};


void geotest()
{
	char str[20];
	static int counter = 1;
	
	AP_EN(1);
	AMP_EN(1);
	DAC_SET(DACMID);
	MUX(1);
	osDelay(200);
	AD7190_Reset();
	AD7190_Calibration();
	AD7190_Setup();
	
	sprintf(str,"%s:%04d",geo.type,counter++);
	GUI_DispStringAt(str,0,600);
	
	step0();
	sprintf(str,"Noise:%.1f",geophone.nois);
	GUI_DispStringAt(str,240,600);
	
	step1();
	sprintf(str,"R:%d",geophone.resi);
	GUI_DispStringAt(str,0,650);
	
	
	step2();
	sprintf(str,"F:%.2f    B:%.3f    S:%.1f",geophone.freq,geophone.damp,geophone.sens);
	GUI_DispStringAt(str,0,700);
	
	
	step3();
	sprintf(str,"D:%.3f  Z:%d",geophone.dist,geophone.impe);
	GUI_DispStringAt(str,0,750);
	
	AD7190_PowerDown();
	AP_EN(0);
	AMP_EN(0);
	MUX(0);
}

void step0()
{
	uint32_t i;
	double val=0;
	uint32_t max=0;
	uint32_t min=0xffffff;
	
	DAC_SET(DACMID);	
	osDelay(T_DELAY);
	for(i=0;i<2048;i++)
	{
		Inbuff[i] = AD7190Read();
		val += Inbuff[i];
		if(Inbuff[i]>max)
			max = Inbuff[i];
		if(Inbuff[i]<min)
			min = Inbuff[i];
	}
	rdft(2048,1,Inbuff);
	//ADCMID = Inbuff[0]/2048;
	geophone.nois	= (max-min)*5000.0/0xffffff;
}

void step1()
{
	double val=0;
	int i;
	unsigned short offset;
	float volt = geo.R*geo.X*geo.M*4*PI*PI*geo.F*geo.F/1000000/geo.S;
	int resi;
	int flag=0;
	
	volt = volt>7 ? 7 :	volt;
	volt = volt*R_ref/geo.R;
	volt = volt>2.2 ? 2.2 : volt;

	
	offset =(unsigned short)(volt/5*0xffff);
	DAC_SET(DACMID+offset);
	osDelay(T_DELAY);
	for(i=0;i<64;i++)
		val+=AD7190Read();
	val /= 64;
	geophone.resi = (uint32_t)round(R_ref*AMPGAIN*abs(val-ADCMID)/(volt/5*0xffffff));
//	geophone.resi = R_ref*abs(val-MIDV)/(volt/5*0xffff);
//	geophone.resi /= 1+0.004*(geophone.temp-param->T);
}

void step2()
{
	int i;
	unsigned int offset;
	unsigned int a1,a2,T,i1;
	float t;
	float volt = 0.35*0.5*geo.R*geo.X*geo.M*4*PI*PI*geo.F*geo.F/1000000/geo.S;
	volt = volt>7 ? 7 :	volt;
	volt = volt*R_ref/geo.R;
	volt = volt>2.2 ? 2.2 : volt;
	
	offset= volt/5*0xffff;
	i1=0;
	a1=a2=ADCMID;
	DAC_SET(DACMID+offset);
	osDelay(T_DELAY);
	AD7190Read();
	DAC_SET(DACMID);
	for(i=0;i<4096;i++)
		Inbuff[i] = AD7190Read();
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
	while(Inbuff[i]>=ADCMID && i<4096)
		i++;
	T = i-2;
	t = log((float)abs(a1-ADCMID)/abs(ADCMID-a2));
	geophone.damp = t/sqrt(PI*PI+t*t);
	t = (float)T/4096;
	geophone.freq = 1.0/(2*t*sqrt(1-geophone.damp*geophone.damp));
	t = sqrt(1-geophone.damp*geophone.damp)/geophone.damp;
	geophone.sens = sqrt(2*PI*geophone.freq*geo.M/1000*abs(a1-ADCMID)*AMPGAIN/0xffffff*5.0/(volt/R_ref)*exp(atan(t)/t));
	//geophone.damp /= 1-0.002*(geophone.temp-geo.T);
	//GUI_DispDecAt(T,400,750,4);
}
	
void step3()
{
	int i;
	int fs = 4096;
	int N = 4096;
	float mag=0.7*0.01778*geo.S*R_ref/geo.Z;
	int NF = geo.DF;
	
	DAC_SWEEP(NF,-mag,0);
  osDelay(T_DELAY);
 	for(i=0;i<N;i++)
		Inbuff[i] = AD7190Read();
	DAC_SWEEP(0,0,0);
	rdft(N,1,Inbuff);
	Inbuff[1] = sqrt(Inbuff[2*NF]*Inbuff[2*NF]+Inbuff[2*NF+1]*Inbuff[2*NF+1]);
	Inbuff[2] = sqrt(Inbuff[4*NF]*Inbuff[4*NF]+Inbuff[4*NF+1]*Inbuff[4*NF+1]);
	Inbuff[3] = sqrt(Inbuff[6*NF]*Inbuff[6*NF]+Inbuff[6*NF+1]*Inbuff[6*NF+1]);
	geophone.impe = (uint32_t)round((R_ref*AMPGAIN*Inbuff[1]/(N/2)/(mag/5*0xffffff)));
	geophone.dist = 100.0*(Inbuff[2]+Inbuff[3])/Inbuff[1];
}
