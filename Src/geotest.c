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
#include "beep.h"

extern void rdft(int n, int isgn, float *a);
extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelvetica32;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelveticaNeueLT48;
extern IWDG_HandleTypeDef IwdgHandle;

uint32_t test0,test1;
uint32_t ADCMID = 0x800000;
static float Inbuff[4096];
struct Geophone geophone;
struct GeoParam geoparam[10];
struct Settings settings;
static struct GeoParam *geo;
float curTemperature;

float shuntResist;
float stringResist;
float lineResist;
float totalResist;

static void step0();
static void step1();
static void step2();
static void step3();
static void polarity();
static void contidrive();

int geotest()
{
	char str[20];
	float portion;
	int fault = 0;
	int COLOR;
	
	geo = &geoparam[settings.paramnum];
	if(settings.sensormode == 3)
		geophone.temp = settings.temperature;
	else if(settings.sensormode == 0)
		geophone.temp = geo->T;
	else
		geophone.temp = curTemperature;
	
	shuntResist = settings.shunt>0 ? (float)settings.shunt*geo->R/(settings.shunt+geo->R) : geo->R;
	stringResist = shuntResist*settings.series/settings.parallel;
	lineResist = settings.lineR*0.001*2*((settings.series-1)*settings.interval/settings.parallel+settings.leadin);
	totalResist = stringResist+lineResist;
	
	GUI_SetColor(WHITE);
	GUI_FillRect(0,260,479,799);
	GUI_SetColor(BLACK);
	GUI_SetBkColor(WHITE);
	GUI_SetFont(&GUI_FontHelvetica32);	
	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_TOP);
	GUI_DispStringAt("Noise",100,260);
	GUI_DispStringAt("Leakage",100,300);
	GUI_DispStringAt("Resistance",100,340);
	GUI_DispStringAt("Frequency",100,380);
	GUI_DispStringAt("Damping",100,420);
	GUI_DispStringAt("Sensitivity",100,460);
	GUI_DispStringAt("Distortion",100,500);
	GUI_DispStringAt("Impedance",100,540);
	GUI_DispStringAt("Polarity",100,580);
	GUI_DispStringAt("Low Drive",100,620);
	
	AP_EN(1);
	AMP_EN(1);
	DAC_SET(DACMID);
	MUX(1);
	HAL_Delay(100);
	AD7190_Reset();
	AD7190_Calibration();
	
	AD7190_Setup(0);
	GUI_SetColor(WHITE);
	step0();
	sprintf(str,"%.1f",geophone.nois);
	GUI_SetBkColor(geophone.nois>10 ? 0x005a62ff : 0x005bc15b);
	GUI_DispStringAt("              ",260,260);
	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_TOP);
	GUI_DispStringAt(str,316,260);
	
	step1();
	if(geophone.leakage>100)
		sprintf(str,">100");
	else
		sprintf(str,"%.1f",geophone.leakage);
	if(geophone.leakage<10)
	{
		fault=1;
		COLOR = 0x005a62ff;
	}
	else
		COLOR = 0x005bc15b;
	GUI_SetBkColor(COLOR);
	GUI_DispStringAt("              ",260,300);
	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_TOP);
	GUI_DispStringAt(str,316,300);
	sprintf(str,"%d",geophone.resi);
	portion = (float)(geophone.resi-geo->R)/geo->R;
	GUI_SetBkColor((portion>geo->Rp|portion<geo->Rn) ? 0x005a62ff : 0x005bc15b);
	GUI_DispStringAt("              ",260,340);
	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_TOP);
	GUI_DispStringAt(str,316,340);
	
	step2();
	sprintf(str,"%.2f",geophone.freq);
	portion = (float)(geophone.freq-geo->F)/geo->F;
	if(geophone.leakage>100)
		sprintf(str,">100");
	else
		sprintf(str,"%.1f",geophone.leakage);
	if(portion>geo->Fp||portion<geo->Fn)
	{
		fault=1;
		COLOR = 0x005a62ff;
	}
	else
		COLOR = 0x005bc15b;
	GUI_SetBkColor(COLOR);
	GUI_DispStringAt("              ",260,380);
	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_TOP);
	GUI_DispStringAt(str,316,380);
	sprintf(str,"%.3f",geophone.damp);
	portion = (float)(geophone.damp-geo->B)/geo->B;
	if(portion>geo->Bp||portion<geo->Bn)
	{
		fault=1;
		COLOR = 0x005a62ff;
	}
	else
		COLOR = 0x005bc15b;
	GUI_SetBkColor(COLOR);
	GUI_DispStringAt("              ",260,420);
	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_TOP);
	GUI_DispStringAt(str,316,420);
	sprintf(str,"%.1f",geophone.sens);
	portion = (float)(geophone.sens-geo->S)/geo->S;
	if(portion>geo->Sp||portion<geo->Sn)
	{
		fault=1;
		COLOR = 0x005a62ff;
	}
	else
		COLOR = 0x005bc15b;
	GUI_SetBkColor(COLOR);
	GUI_DispStringAt("              ",260,460);
	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_TOP);
	GUI_DispStringAt(str,316,460);
	if((portion>geo->Fp|portion<geo->Fn) || (portion>geo->Bp|portion<geo->Bn) || (portion>geo->Sp|portion<geo->Sn)) fault=1;
	
	step3();
	sprintf(str,"%.3f",geophone.dist);
	if(geophone.dist>geo->D)
	{
		fault=1;
		COLOR = 0x005a62ff;
	}
	else
		COLOR = 0x005bc15b;
	GUI_SetBkColor(COLOR);
	GUI_DispStringAt("              ",260,500);
	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_TOP);
	GUI_DispStringAt(str,316,500);
	sprintf(str,"%d",geophone.impe);
	portion = (float)(geophone.impe-geo->Z)/geo->Z;
	if(portion>geo->Zp||portion<geo->Zn)
	{
		fault=1;
		COLOR = 0x005a62ff;
	}
	else
		COLOR = 0x005bc15b;
	GUI_SetBkColor(COLOR);
	GUI_DispStringAt("              ",260,540);
	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_TOP);
	GUI_DispStringAt(str,316,540);
	if(geophone.dist>geo->D || (portion>geo->Zp|portion<geo->Zn)) fault=1;
	
	if(settings.polarity)
		polarity();
	if(geophone.polarity==-1) fault=1;
	
	if(settings.ldrate)
		contidrive();
	
	AD7190_PowerDown();
	AMP_EN(0);
	AP_EN(0);
	MUX(0);
	return fault;
}

static void step0()
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

static void step1()
{
	double val=0;
	double val2=0;
	int i;
	unsigned short offset;
	float volt = 0.7*geo->R*geo->X*geo->M*4*PI*PI*geo->F*geo->F/geo->S;
	volt = volt*settings.series*totalResist/stringResist;
	volt = volt*R_ref/totalResist;
	if(volt>2.45) volt=2.45;
	
	offset =(unsigned short)(volt/5*0xffff);
	DAC_SET(DACMID+offset);
	HAL_Delay(T_DELAY);
	
	
dbg("step1 0");
	for(i=0;i<64;i++)
		val+=AD7190Read();
dbg("step1 1");
	val /= 64;
	geophone.resi = (uint32_t)round(R_ref*AMPGAIN*abs(val-ADCMID)/(volt/5*0xffffff));
	geophone.resi = geophone.resi-lineResist;
	geophone.resi /= 1+0.004*(geophone.temp-geo->T);
	
	
	AD7190_Setup(1);
	HAL_Delay(100);
	for(i=0;i<64;i++)
		val2+=AD7190Read();
	val2 /= 64;
	geophone.leakage = val2<=0x800000 ? 200 : (AMPGAIN*abs(val-ADCMID)*5.0/0xffffff+2.5)/((val2-0x800000)*5.0/0xffffff)*0.03-0.2;
//while(1);
	AD7190_Setup(0);
}

static void step2()
{
	int i;
	unsigned int offset;
	unsigned int a1,a2,T,i1;
	float t;
	float volt = 0.35*0.5*geo->R*geo->X*geo->M*4*PI*PI*geo->F*geo->F/geo->S;
	volt = volt*settings.series*totalResist/stringResist;
	volt = volt*R_ref/totalResist;
	if(volt>2.45) volt=2.45;
	
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
	if(geophone.damp<0) geophone.damp = 0;
	t = (float)T/4096;
	geophone.freq = 1.0/(2*t*sqrt(1-geophone.damp*geophone.damp));
	t = sqrt(1-geophone.damp*geophone.damp)/geophone.damp;
	geophone.sens = sqrt(2*PI*geophone.freq*geo->M*settings.series*settings.parallel*abs(a1-ADCMID)*AMPGAIN/0xffffff*5.0/(volt/R_ref)*exp(atan(t)/t));
	geophone.damp /= 1-0.002*(geophone.temp-geo->T);
}
	
static void step3()
{
	int i;
	int fs = 4096;
	int N = 4096;
	float mag;
	int NF = geo->DF;
	float shuntZ = settings.shunt>0 ? (float)(settings.shunt*geo->Z)/(settings.shunt+geo->Z) : geo->Z;
	float stringZ = shuntZ*settings.series/settings.parallel;
	float totalZ = stringZ+lineResist;
	mag = 0.7*0.01778*geo->S;
	if(!settings.constant)
		mag = mag*geo->DF/12;
	mag = mag*settings.series;
	mag = mag*R_ref/totalZ;
	if(mag>2.45) mag=2.45;
	
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
	geophone.impe = round(R_ref*AMPGAIN*Inbuff[1]/(N/2)/(mag/5*0xffffff));
	geophone.impe -= lineResist;
	geophone.dist = 100.0*(Inbuff[2]+Inbuff[3])/Inbuff[1];
}
	
struct UIWidget next = {0,1,1,{260,700,459,760},"Next",0,NULL,NULL,drawButton,NULL};
static void polarity()
{
	int i;
	unsigned int temp;
	unsigned int vmin,vmax,imax,imin;
	float max,min;
	char str[20];
	
	next.enable =1;
	drawButton(&next);
	
	geophone.polarity = 0;
	GUI_SetColor(WHITE);
	GUI_SetBkColor(0x002fbeff);
	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_TOP);
	GUI_DispStringAt("              ",260,580);
	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_TOP);
	GUI_DispStringAt("0",316,580);
	while(1)
	{
		HAL_IWDG_Refresh(&IwdgHandle);
		vmin=0xffffff;
		vmax=imax=imin=0;
		for(i=0;i<2000;i++)
		{
			temp = AD7190Read();
			if(temp>vmax)
			{
				vmax=temp;
				imax=i;
			}
			if(temp<vmin)
			{
				vmin=temp;
				imin=i;
			}
			if((GPIOA->IDR&0x7) == 0x4)
			{
				next.enable = 0;
				drawButton(&next);
				beep(200);
				return;
			}
		}
		max = abs(vmax-0x800000)*AMPGAIN*5.0/0xffffff/settings.series;
		min = abs(0x800000-vmin)*AMPGAIN*5.0/0xffffff/settings.series;
		if(max>0.5 && max<2.4 && min>0.5 && min<2.4)
		{
			if(imax<imin)
			{
				beep(50);HAL_Delay(50);beep(50);HAL_Delay(50);beep(50);
				geophone.polarity=-1;
			}
			else if(imax>imin)
			{
				beep(250);
				geophone.polarity=1;
			}
			sprintf(str,"%d",geophone.polarity);
			GUI_SetBkColor(geophone.polarity!=1 ? 0x005a62ff : 0x005bc15b);
			GUI_DispStringAt("              ",260,580);
			GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_TOP);
			GUI_DispStringAt(str,316,580);
		}
	}
}

static void contidrive()
{
	int i;
	int fs = 4096;
	int N = 4096;
	float mag;
	int NF = geo->DF;
	float shuntZ = settings.shunt>0 ? (float)(settings.shunt*geo->Z)/(settings.shunt+geo->Z) : geo->Z;
	float stringZ = shuntZ*settings.series/settings.parallel;
	float totalZ = stringZ+lineResist;
	float impedance;
	int flag = 1;
	char str[20];
	
	next.enable =1;
	drawButton(&next);
	GUI_SetColor(WHITE);
	GUI_SetBkColor(0x002fbeff);
	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_TOP);
	GUI_DispStringAt("              ",260,620);
	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_TOP);
	GUI_DispStringAt("0-0",316,620);
	
	geophone.maxZ = 0;
	geophone.minZ = 0xffffffff;
	
	mag = 0.7*0.01778*geo->S;
	if(!settings.constant)
		mag = mag*geo->DF/12;
	mag = mag*settings.series;
	mag = mag*R_ref/totalZ;
	mag = mag*settings.ldrate/100;
	if(mag>2.45) mag=2.45;
	
	DAC_SWEEP(NF,-mag,0);
  HAL_Delay(T_DELAY);
	while(flag)
	{
		HAL_IWDG_Refresh(&IwdgHandle);
		for(i=0;i<N;i++)
		{
			Inbuff[i] = AD7190Read();
			if((GPIOA->IDR&0x7) == 0x4)
			{
				next.enable =0;
				drawButton(&next);
				beep(200);
				flag = 0;
				break;
			}
		}
		if(flag)
		{
			rdft(N,1,Inbuff);
			Inbuff[1] = sqrt(Inbuff[2*NF]*Inbuff[2*NF]+Inbuff[2*NF+1]*Inbuff[2*NF+1]);
			impedance = round(R_ref*AMPGAIN*Inbuff[1]/(N/2)/(mag/5*0xffffff));
			impedance -= lineResist;
			if(geophone.maxZ < impedance)
				geophone.maxZ = impedance;
			if(geophone.minZ > impedance)
				geophone.minZ = impedance;
			
			sprintf(str,"%d-%d",geophone.minZ,geophone.maxZ);
			GUI_SetColor(WHITE);
			GUI_SetBkColor(0x002fbeff);
			GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_TOP);
			GUI_DispStringAt(str,260,620);
			GUI_SetBkColor(WHITE);
			GUI_DispCEOL();
		}
	}
	DAC_SWEEP(0,0,0);
}
