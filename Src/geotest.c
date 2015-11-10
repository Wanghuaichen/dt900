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
extern struct UIInfo UIInfo;
extern int WHOLE_FLASH_ERASE;

uint32_t test0,test1;
uint32_t ADCMID = 0x800000;
static float Inbuff[4096];
struct Geophone geophone;
struct GeoParam geoparam[10];
volatile struct Settings settings;
static struct GeoParam *geo;
float curTemperature;

static int openshort();
static void step0();
static void step1();
static void step2();
static void step3();
static void polarity();
static void contidrive();

long encode()
{
	double passwd;
	passwd = UIInfo.uid[0]+0xd0+UIInfo.uid[1]*UIInfo.uid[4]+UIInfo.uid[7]+UIInfo.uid[3]*3;
	passwd = sqrt(passwd)+(double)UIInfo.uid[6]/2+(double)UIInfo.uid[5]/3;
	passwd = (passwd*100-(int)(passwd*100))*1000000000;
	return (long)passwd;
}

void verify()
{
	if(rand()%100<5 && encode()!=*((long *)0x081C0000))
		settings.sensormode = 4;
}

void analog(int option)
{
	if(option)
	{
		AP_EN(1);
		AMP_EN(1);
		DAC_SET(DACMID);
		MUX(1);
		HAL_Delay(100);
		AD7190_Reset();
		AD7190_Calibration();
		AD7190_Setup(0);
	}
	else
	{
		AD7190_PowerDown();
		AMP_EN(0);
		AP_EN(0);
		MUX(0);
	}
}

int geotest()
{
	char str[20];
	char blank[10];
	float portion;
	int COLOR;
	int openshorttest;
	
	sprintf(blank,"              ");
	memset(&geophone,0,sizeof(geophone));
	geo = &geoparam[settings.paramnum];
	if(settings.sensormode == 3)
		geophone.temp = settings.temperature;
	else if(settings.sensormode == 0)
		geophone.temp = geo->T;
	else
		geophone.temp = curTemperature;
	
	geophone.rnom = (uint32_t)((settings.shunt>0 ? (float)settings.shunt*geo->R/(settings.shunt+geo->R) : geo->R)*settings.series/settings.parallel);
	geophone.rmax = (uint32_t)(geophone.rnom*(1+geo->Rp));
	geophone.rmin = (uint32_t)(geophone.rnom*(1-geo->Rn));
	geophone.fnom = geo->F;
	geophone.fmax = geophone.fnom*(1+geo->Fp);
	geophone.fmin = geophone.fnom*(1-geo->Fn);
	geophone.bnom = settings.shunt>0 ? geo->S*geo->S/4/3.1415926/geo->M/geo->F/(geo->R+settings.shunt)+geo->B : geo->B;
	geophone.bmax = geophone.bnom*(1+geo->Bp);
	geophone.bmin = geophone.bnom*(1-geo->Bn);
	geophone.snom = (settings.shunt>0 ? geo->S*settings.shunt/(settings.shunt+geo->R) : geo->S)*settings.series;
	geophone.smax = geophone.snom*(1+geo->Sp);
	geophone.smin = geophone.snom*(1-geo->Sn);
	geophone.znom = (uint32_t)((settings.shunt>0 ? (float)geo->Z*settings.shunt/(geo->Z+settings.shunt) : geo->Z)*settings.series/settings.parallel);
	geophone.zmax = (uint32_t)(geophone.znom*(1+geo->Zp));
	geophone.zmin = (uint32_t)(geophone.znom*(1-geo->Zn));
	geophone.rline = settings.lineR*0.001*2*((settings.series-1)*settings.interval/settings.parallel+settings.leadin);
	geophone.rtotal = geophone.rnom+geophone.rline;
	geophone.ztotal = geophone.znom+geophone.rline;
	
	GUI_SetColor(BLACK);
	GUI_SetBkColor(WHITE);
	GUI_SetFont(&GUI_FontHelvetica32);	
	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_TOP);
	GUI_DispStringAt("Noise(mV)",60,260);
	GUI_DispStringAt("Leakage(M})",60,300);
	GUI_DispStringAt("Resistance(})",60,340);
	GUI_DispStringAt("Frequency(Hz)",60,380);
	GUI_DispStringAt("Damping",60,420);
	if(settings.units)
		GUI_DispStringAt("Sensitivity(V/\"/s)",60,460);
	else
		GUI_DispStringAt("Sensitivity(V/m/s)",60,460);
	GUI_DispStringAt("Distortion(%)",60,500);
	GUI_DispStringAt("Impedance(})",60,540);
	GUI_DispStringAt("Polarity",60,580);
	GUI_DispStringAt("Low Drive(})",60,620);
	
	analog(1);
	GUI_SetColor(WHITE);
	openshorttest = openshort();
	if(openshorttest!=0)
	{
		analog(0);
		sprintf(str,openshorttest==1 ? "OPEN" : "SHORT");
		GUI_SetBkColor(0x005a62ff);
		GUI_SetTextAlign(GUI_TA_HCENTER);
		GUI_DispStringAt("              ",356,260);
		GUI_SetTextAlign(GUI_TA_HCENTER);
		GUI_DispStringAt(str,356,260);
		geophone.fault = openshorttest==1 ? -1 : -2;
		return geophone.fault;
	}
	
	step0();
	sprintf(str,"%.1f",geophone.nois);
	GUI_SetBkColor(geophone.nois>10 ? 0x005a62ff : 0x005bc15b);
	GUI_SetTextAlign(GUI_TA_HCENTER);
	GUI_DispStringAt("              ",356,260);
	GUI_SetTextAlign(GUI_TA_HCENTER);
	GUI_DispStringAt(str,356,260);
	
	step1();
	if(geophone.leakage>100)
		sprintf(str,">100");
	else
		sprintf(str,"%.1f",geophone.leakage);
	if(geophone.leakage<10)
	{
		geophone.fault=1;
		COLOR = 0x005a62ff;
	}
	else
		COLOR = 0x005bc15b;
	GUI_SetBkColor(COLOR);
	GUI_DispStringAt("              ",300,300);
	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_TOP);
	GUI_DispStringAt(str,356,300);
	
	sprintf(str,"%d",(int)geophone.resi);
	if(geophone.resi>geophone.rmax||geophone.resi<geophone.rmin)
	{
		geophone.fault=1;
		COLOR = 0x005a62ff;
	}
	else
		COLOR = 0x005bc15b;
	GUI_SetBkColor(COLOR);
	GUI_DispStringAt("              ",300,340);
	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_TOP);
	GUI_DispStringAt(str,356,340);
	
	step2();
	sprintf(str,"%.2f",geophone.freq);
	if(geophone.fnom>geophone.fmax||geophone.fnom<geophone.fmin)
	{
		geophone.fault=1;
		COLOR = 0x005a62ff;
	}
	else
		COLOR = 0x005bc15b;
	GUI_SetBkColor(COLOR);
	GUI_DispStringAt("              ",300,380);
	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_TOP);
	GUI_DispStringAt(str,356,380);
	sprintf(str,"%.3f",geophone.damp);
	if(geophone.damp>geophone.bmax||geophone.damp<geophone.bmin)
	{
		geophone.fault=1;
		COLOR = 0x005a62ff;
	}
	else
		COLOR = 0x005bc15b;
	GUI_SetBkColor(COLOR);
	GUI_DispStringAt("              ",300,420);
	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_TOP);
	GUI_DispStringAt(str,356,420);
	if(settings.units)
		sprintf(str,"%.3f",(int)(geophone.sens/39.37*1000+0.1)/1000.0);
	else
		sprintf(str,"%.1f",geophone.sens);
	if(geophone.sens>geophone.smax||geophone.sens<geophone.smin)
	{
		geophone.fault=1;
		COLOR = 0x005a62ff;
	}
	else
		COLOR = 0x005bc15b;
	GUI_SetBkColor(COLOR);
	GUI_DispStringAt("              ",300,460);
	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_TOP);
	GUI_DispStringAt(str,356,460);
	
	step3();
	sprintf(str,"%.3f",geophone.dist);
	if(geophone.dist>geo->D)
	{
		geophone.fault=1;
		COLOR = 0x005a62ff;
	}
	else
		COLOR = 0x005bc15b;
	GUI_SetBkColor(COLOR);
	GUI_DispStringAt("              ",300,500);
	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_TOP);
	GUI_DispStringAt(str,356,500);
	sprintf(str,"%d",(int)geophone.impe);
	if(geophone.impe>geophone.zmax||geophone.impe<geophone.zmin)
	{
		geophone.fault=1;
		COLOR = 0x005a62ff;
	}
	else
		COLOR = 0x005bc15b;
	GUI_SetBkColor(COLOR);
	GUI_DispStringAt("              ",300,540);
	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_TOP);
	GUI_DispStringAt(str,356,540);
	
	if(settings.polarity)
		polarity();
	if(geophone.polarity==-1)
		geophone.fault = 1;
	
	if(settings.ldrate)
		contidrive();
	if(geophone.minZ<geophone.zmin || geophone.maxZ>geophone.zmax)
		geophone.fault = 1;
	
	analog(0);
	
	return geophone.fault;
}



static int openshort()
{
	double val=0;
	int i,ret;
	unsigned short offset;
	float volt;
	
	verify();
	
	volt =0.07;
	offset =(unsigned short)(volt/5*0xffff);
	DAC_SET(DACMID+offset);
	HAL_Delay(100);
	for(i=0;i<64;i++)
		val+=AD7190Read();
	val /= 64;
	val = R_ref*AMPGAIN*abs(val-0x800000)/(volt/5*0xffffff)-0.25;
	
	return val>50000 ? 1 : val<5 ? -1 : 0;
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
	volt = volt*settings.series*geophone.rtotal/geophone.rnom;
	if(volt>9) volt=9;
	volt = volt*R_ref/geophone.rtotal;
	if(volt>2.45) volt=2.45;
	
	offset =(unsigned short)(volt/5*0xffff);
	DAC_SET(DACMID+offset);
	HAL_Delay(T_DELAY);
	
	
dbg("step1 0");
	for(i=0;i<64;i++)
		val+=AD7190Read();
dbg("step1 1");
	val /= 64;
	geophone.resi = (uint32_t)round(R_ref*AMPGAIN*abs(val-ADCMID)/(volt/5*0xffffff)-0.25);
	geophone.resi = geophone.resi-geophone.rline;
	geophone.resi /= 1+0.004*(geophone.temp-geo->T);
	
	
	AD7190_Setup(1);
	for(i=0;i<64;i++)
		val2+=AD7190Read();
	val2 /= 64;
	geophone.leakage = val2<=0x800000 ? 100.001 : (AMPGAIN*abs(val-ADCMID)*5.0/0xffffff+2.5)/((val2-0x800000)*5.0/0xffffff)*0.03-0.2;
//while(1);
	if(geophone.leakage>100)
		geophone.leakage = 100.001;
	AD7190_Setup(0);
}

static void step2()
{
	int i;
	unsigned int offset;
	unsigned int a1,a2,T,i1;
	float t;
	float volt = 0.35*0.5*geo->R*geo->X*geo->M*4*PI*PI*geo->F*geo->F/geo->S;
	volt = volt*settings.series*geophone.rtotal/geophone.rnom;
	if(volt>9) volt=9;
	volt = volt*R_ref/geophone.rtotal;
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
	t = T/4096.0;
	geophone.freq = 1.0/(2*t*sqrt(1-geophone.damp*geophone.damp));
	t = sqrt(1-geophone.damp*geophone.damp)/geophone.damp;
	geophone.sens = sqrt(2*PI*geophone.freq*geo->M*settings.series*settings.parallel*abs(a1-ADCMID)*AMPGAIN/0xffffff*5.0/(volt/R_ref)*exp(atan(t)/t));
	geophone.damp /= 1-0.002*(geophone.temp-geo->T);
}
	
static void step3()
{
	int i;
//	int fs = 4096;
	int N = 4096;
	float mag;
	int NF = geo->DF;
	mag = geo->speed*0.0254*geophone.snom;
	if(!settings.constant)
		mag = mag*geo->DF/12;
	if(mag>9) mag=9;
	mag = mag*R_ref/geophone.ztotal;
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
	geophone.impe -= geophone.rline;
	geophone.dist = 100.0*(Inbuff[2]+Inbuff[3])/Inbuff[1];
}
	
struct UIWidget next = {0,1,1,{260,700,459,760},"Next",0,NULL,NULL,drawButton,NULL};
//static void polarity()
//{
//	int i;
//	int temp;
//	int vmin,vmax,imax,imin;
//	float max,min;
//	char str[20];
//	
//	float volt = 0.5*geo->R*geo->X*geo->M*4*PI*PI*geo->F*geo->F/sens*settings.series;
//	
//	UITouchClear();
//	next.enable =1;
//	drawButton(&next);
//	
//	geophone.polarity = 0;
//	GUI_SetColor(WHITE);
//	GUI_SetBkColor(0x002fbeff);
//	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_TOP);
//	GUI_DispStringAt("              ",300,580);
//	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_TOP);
//	GUI_DispStringAt("0",356,580);
//	while(1)
//	{
//HAL_IWDG_Refresh(&IwdgHandle);
//vmax=0;
//for(i=0;i<2000;i++)
//{
//	temp = AD7190Read()-ADCMID;
//	if(abs(temp)>abs(vmax))
//		vmax = temp;
//	
//	tpScan();
//	if(UIInfo.TouchEvent==TOUCH_DISPLACEMENT && UIInfo.tpX<=459 && UIInfo.tpX>=260 && UIInfo.tpY<=760 && UIInfo.tpY>=700)
//	{
//		next.enable = 0;
//		drawButton(&next);
//		return;
//	}	
//	if((GPIOA->IDR&0x7) == 0x4)
//	{
//		next.enable = 0;
//		drawButton(&next);
//		return;
//	}
//}
//max = abs(vmax*AMPGAIN*5.0/0xffffff);
//if(max>0.1 && max<8)
//{
//	if(vmax<0)
//	{
//		beep(50);HAL_Delay(50);beep(50);HAL_Delay(50);beep(50);
//		geophone.polarity=-1;
//	}
//	else
//	{
//		beep(250);
//		geophone.polarity=1;
//	}
//	sprintf(str,"%d",geophone.polarity);
//	GUI_SetBkColor(geophone.polarity!=1 ? 0x005a62ff : 0x005bc15b);
//	GUI_DispStringAt("              ",300,580);
//	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_TOP);
//	GUI_DispStringAt(str,356,580);
//}
//	}
//}


//static void polarity()
//{
//	int i;
//	int temp;
//	int vmin,vmax,imax,imin;
//	float max,min;
//	char str[20];
//	
//	UITouchClear();
//	next.enable =1;
//	drawButton(&next);
//	
//	geophone.polarity = 0;
//	GUI_SetColor(WHITE);
//	GUI_SetBkColor(0x002fbeff);
//	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_TOP);
//	GUI_DispStringAt("              ",300,580);
//	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_TOP);
//	GUI_DispStringAt("0",356,580);
//	while(1)
//	{
//HAL_IWDG_Refresh(&IwdgHandle);
//vmax=vmin=ADCMID;
//imax=imin=0;
//for(i=0;i<2000;i++)
//{
//	temp = AD7190Read();
//	if(temp>vmax)
//	{
//		vmax = temp;
//		imax = i;
//	}
//	else if(temp<vmin)
//	{
//		vmin = temp;
//		imin = i;
//	}
//	
//	tpScan();
//	if(UIInfo.TouchEvent==TOUCH_DISPLACEMENT && UIInfo.tpX<=459 && UIInfo.tpX>=260 && UIInfo.tpY<=760 && UIInfo.tpY>=700)
//	{
//		next.enable = 0;
//		drawButton(&next);
//		return;
//	}	
//	if((GPIOA->IDR&0x7) == 0x4)
//	{
//		next.enable = 0;
//		drawButton(&next);
//		return;
//	}
//}
//max = abs((vmax-ADCMID)*AMPGAIN*5.0/0xffffff);
//min = abs((vmin-ADCMID)*AMPGAIN*5.0/0xffffff);
//if(max>0.1 && min>0.1 && max<8 && min <8)
//{
//	if(imax>imin)
//	{
//		beep(50);HAL_Delay(50);beep(50);HAL_Delay(50);beep(50);
//		geophone.polarity=-1;
//	}
//	else
//	{
//		beep(250);
//		geophone.polarity=1;
//	}
//	sprintf(str,"%d",geophone.polarity);
//	GUI_SetBkColor(geophone.polarity!=1 ? 0x005a62ff : 0x005bc15b);
//	GUI_DispStringAt("              ",300,580);
//	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_TOP);
//	GUI_DispStringAt(str,356,580);
//}
//	}
//}

static void polarity()
{
	int i;
	int temp;
	int flag,pol,cnt;
	int max,min,th;
	char str[20];
	
	UITouchClear();
	next.enable =1;
	drawButton(&next);
	
	geophone.polarity = 0;
	GUI_SetColor(WHITE);
	GUI_SetBkColor(0x002fbeff);
	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_TOP);
	GUI_DispStringAt("              ",300,580);
	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_TOP);
	GUI_DispStringAt("0",356,580);
	th = 4000*geo->S;
	while(1)
	{
HAL_IWDG_Refresh(&IwdgHandle);
flag=pol=cnt=0;
max=0;
min=0xffffff;
for(i=0;i<2000;i++)
{
	temp = AD7190Read();
	if(flag==-1)
	{
		if(temp>max) max = temp;
		if(temp<min) min = temp;
	}
	else if(flag==1)
	{
//		if(abs(temp-ADCMID)>67109)
		if(abs(temp-ADCMID)>th)
		{
			if(temp<ADCMID)
				pol=1;
			else
				pol=-1;
			flag=-1;
		}
	}
	else if(flag==0)
	{
		if(abs(temp-ADCMID)<th)
			cnt++;
		if(cnt>10)
			flag=1;
	}
	
	tpScan();
	if(UIInfo.TouchEvent==TOUCH_DISPLACEMENT && UIInfo.tpX<=459 && UIInfo.tpX>=260 && UIInfo.tpY<=760 && UIInfo.tpY>=700)
	{
		next.enable = 0;
		drawButton(&next);
		return;
	}	
	if((GPIOA->IDR&0x7) == 0x4)
	{
		next.enable = 0;
		drawButton(&next);
		return;
	}
}

	if(pol && (max-ADCMID)>th && (ADCMID-min)>th)
	{
		if(pol==1)
		{
			beep(50);HAL_Delay(50);beep(50);HAL_Delay(50);beep(50);
			geophone.polarity=-1;
		}
		else if(pol==-1)
		{
			beep(250);
			geophone.polarity=1;
		}
		sprintf(str,"%d",geophone.polarity);
		GUI_SetBkColor(geophone.polarity!=1 ? 0x005a62ff : 0x005bc15b);
		GUI_DispStringAt("              ",300,580);
		GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_TOP);
		GUI_DispStringAt(str,356,580);
	}
}
}


//static void polarity()
//{
//	int i;
//	int temp;
//	int cntp,cntn,pol;
//	char str[20];
//	
//	UITouchClear();
//	next.enable =1;
//	drawButton(&next);
//	
//	geophone.polarity = 0;
//	GUI_SetColor(WHITE);
//	GUI_SetBkColor(0x002fbeff);
//	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_TOP);
//	GUI_DispStringAt("              ",300,580);
//	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_TOP);
//	GUI_DispStringAt("0",356,580);
//	while(1)
//	{
//HAL_IWDG_Refresh(&IwdgHandle);
//cntp=cntn=pol=0;
//for(i=0;i<2000;i++)
//{
//	temp = AD7190Read();
//	if(abs(temp-ADCMID)>10000)
//	{
//		if(temp>ADCMID)
//			cntp++;
//		else
//			cntn++;
//	}
//	
//	tpScan();
//	if(UIInfo.TouchEvent==TOUCH_DISPLACEMENT && UIInfo.tpX<=459 && UIInfo.tpX>=260 && UIInfo.tpY<=760 && UIInfo.tpY>=700)
//	{
//		next.enable = 0;
//		drawButton(&next);
//		return;
//	}	
//	if((GPIOA->IDR&0x7) == 0x4)
//	{
//		next.enable = 0;
//		drawButton(&next);
//		return;
//	}
//}
//	if(cntp>10 && cntn>10)
//	{
//		if(cntp>cntn)
//		{
//			beep(50);HAL_Delay(50);beep(50);HAL_Delay(50);beep(50);
//			geophone.polarity=-1;
//		}
//		else
//		{
//			beep(250);
//			geophone.polarity=1;
//		}
//		GUI_DispStringAt("3",0,700);
//	}
//	sprintf(str,"%d",geophone.polarity);
//	GUI_SetBkColor(geophone.polarity!=1 ? 0x005a62ff : 0x005bc15b);
//	GUI_DispStringAt("              ",300,580);
//	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_TOP);
//	GUI_DispStringAt(str,356,580);
//}
//}


static void contidrive()
{
	int i;
	int fs = 4096;
	int N = 4096;
	float mag;
	int NF = geo->DF;
	float impedance;
	int flag = 1;
	char str[20];
	
	UITouchClear();
	next.enable =1;
	drawButton(&next);
	
	GUI_SetColor(WHITE);
	GUI_SetBkColor(0x002fbeff);
	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_TOP);
	GUI_DispStringAt("              ",300,620);
	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_TOP);
	GUI_DispStringAt("0-0",356,620);
	
	geophone.maxZ = 0;
	geophone.minZ = 0xffffffff;
	
	mag = geo->speed*0.0254*geophone.snom;
	if(!settings.constant)
		mag = mag*geo->DF/12;
	mag = mag*settings.ldrate/100;
	if(mag>9) mag=9;
	mag = mag*R_ref/geophone.ztotal;
	if(mag>2.45) mag=2.45;
	
	DAC_SWEEP(NF,-mag,0);
  HAL_Delay(T_DELAY);
	while(flag)
	{
		HAL_IWDG_Refresh(&IwdgHandle);
		for(i=0;i<N;i++)
		{
			Inbuff[i] = AD7190Read();
			tpScan();
			if(UIInfo.TouchEvent==TOUCH_DISPLACEMENT && UIInfo.tpX<=459 && UIInfo.tpX>=260 && UIInfo.tpY<=760 && UIInfo.tpY>=700)
			{
				next.enable = 0;
				drawButton(&next);
				flag = 0;
				break;
			}	
			if((GPIOA->IDR&0x7) == 0x4)
			{
				next.enable =0;
				drawButton(&next);
				flag = 0;
				break;
			}
		}
		if(flag)
		{
			rdft(N,1,Inbuff);
			Inbuff[1] = sqrt(Inbuff[2*NF]*Inbuff[2*NF]+Inbuff[2*NF+1]*Inbuff[2*NF+1]);
			impedance = round(R_ref*AMPGAIN*Inbuff[1]/(N/2)/(mag/5*0xffffff));
			impedance -= geophone.rline;
			if(geophone.maxZ < impedance)
				geophone.maxZ = impedance;
			if(geophone.minZ > impedance)
				geophone.minZ = impedance;
			
			sprintf(str,"%d-%d",geophone.minZ,geophone.maxZ);
			if(geophone.minZ<geophone.zmin || geophone.maxZ>geophone.zmax)
			{
				geophone.fault=1;
				GUI_SetBkColor(0x005a62ff);
			}
			else
				GUI_SetBkColor(0x005bc15b);
			GUI_SetColor(WHITE);
			GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_TOP);
			GUI_DispStringAt(str,300,620);
			GUI_SetBkColor(WHITE);
			GUI_DispCEOL();
		}
	}
	DAC_SWEEP(0,0,0);
}

int ztest(int freq)
{
	int i;
	int N=4096;
	float mag = 0.01;
	HAL_IWDG_Refresh(&IwdgHandle);
	DAC_SWEEP(freq,-mag,0);
  HAL_Delay(800);
	for(i=0;i<N;i++)
	{
		Inbuff[i] = AD7190Read();
	}
	DAC_SWEEP(0,0,0);
	
	rdft(N,1,Inbuff);
	Inbuff[1] = sqrt(Inbuff[2*freq]*Inbuff[2*freq]+Inbuff[2*freq+1]*Inbuff[2*freq+1]);
	return (int)round(R_ref*AMPGAIN*Inbuff[1]/(N/2)/(mag/5*0xffffff));
}
	
