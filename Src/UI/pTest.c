#include "ui.h"
#include "geotest.h"
#include "flash.h"
#include "ff_gen_drv.h"
#include "rtc.h"
#include "ostask.h"
#include "cyma568.h"
#include "beep.h"

extern struct UIPage pSetup;
extern struct GeoParam geoparam[10];
extern struct Settings settings;
extern struct Geophone geophone;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelvetica32;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelveticaNeueLT48;
extern struct UIInfo UIInfo;
extern RTC_DateTypeDef rtcDate;
extern RTC_TimeTypeDef rtcTime;
extern struct UIPage pKeyboard;
extern struct KBInfo kbInfo;

static char StringArray[2][20];

static struct UIWidget widgetList[4];
struct UIPage pTest;
static int testflag=0;
static int saveflag=0;

static void kbCallBack()
{
	char str[20];
	int val;
	switch(pTest.widgetSelected)
	{
		case 2:
			val = atoi(kbInfo.kbBuff);
			settings.serialno = val<0 ? 0 : val;
			break;
		case 3:
			strcpy(settings.code,kbInfo.kbBuff);
			break;
		default:
			break;
	}
}

static void goSubSettings(struct UIWidget * widget)
{
	kbInfo.kbParent = &pTest;
	strcpy(kbInfo.kbTitle,widget->widgetTitle);
	strcpy(kbInfo.kbBuff,widget->widgetPtr);
	kbInfo.kbCallBack = kbCallBack;
	switch(widget->widgetIndex)
	{
		case 2:
			kbInfo.strlength = 6;
			break;
		case 3:
			kbInfo.strlength = 11;
			break;
		default:
			break;
	}
	PageJump(&pKeyboard);
}


static void header(FIL* pMyFile,char* str,UINT* pwbytes)
{
	if(saveflag)
		return;
	saveflag = 1;
	sprintf(str,"*****************************************************************************************************************************\r\n");
	f_write(pMyFile, str, strlen(str),pwbytes);
//	sprintf(str,"Device\tSCG900\tPID\t%s\tSetup\t%s\r\n",UIInfo.uid,geoparam[settings.paramnum].type);
//	f_write(pMyFile, str, strlen(str),pwbytes);
//	sprintf(str,"Shunt\t%d\tSeries\t%d\tParallel\t%d\r\n",settings.shunt,settings.series,settings.parallel);
//	f_write(pMyFile, str, strlen(str),pwbytes);
////	sprintf(str,"Line Resistance\t%d\tLead-in\t%d\tInterval\t%d\r\n",settings.lineR,settings.leadin,settings.interval);
////	f_write(pMyFile, str, strlen(str),pwbytes);
//	sprintf(str,"Temp. Correct.\t%s\t@Constant\t%s\r\n\r\n",settings.sensormode==0 ? "OFF" : settings.sensormode==1 ? "INNER" : settings.sensormode==2 ? "OUTTER" : "MANUAL",settings.constant ? "VELOCITY" : "EXCURSION");
//	f_write(pMyFile, str, strlen(str),pwbytes);
//	sprintf(str,"No.\tDate\tTime\tTemperature(C)\tNoise(mV)\tLeakage(Mohm)\tResistance(ohm)\tFrequency(Hz)\tDamping(-)\tSensitivity(V/m/s)\tDistortion(%%)\tImpedance(ohm)\tPolarity\tDescription\r\n");
//	f_write(pMyFile, str, strlen(str),pwbytes);
}
	
static void save()
{
	FIL MyFile;
	char str[200];
	UINT wbytes;
	
	sprintf(widgetList[1].widgetTitle,"Test");
	if(settings.iteration==1)
	{
		widgetList[0].enable = 0;
		widgetList[0].active = 0;
		widgetList[0].widgetDraw(&widgetList[0]);
	}
	widgetList[1].active = 1;
	pTest.widgetSelected = 1;
	widgetList[1].widgetDraw(&widgetList[1]);
	sprintf(str,"0:%s.geo",settings.filename);
	if(f_open(&MyFile,(TCHAR *)str,FA_OPEN_ALWAYS | FA_WRITE) != FR_OK)
		return;
  f_lseek(&MyFile,MyFile.fsize); 
	header(&MyFile,str,&wbytes);
  sprintf(str,"%05d\t%02d-%02d-%02d\t%02d:%02d:%02d\t%.1f\t%.1f\t%.1f\t%d\t%.2f\t%.3f\t%.1f\t%.3f\t%d\t%d\t%s\r\n",
	settings.serialno,
	rtcDate.Year,rtcDate.Month,rtcDate.Date,
	rtcTime.Hours, rtcTime.Minutes, rtcTime.Seconds,
	geophone.temp,
	geophone.nois,
	geophone.leakage,
	(int)geophone.resi,
	geophone.freq,
	geophone.damp,
	geophone.sens,
	geophone.dist,
	(int)geophone.impe,
	geophone.polarity,
	geophone.fault ==-1 ? "OPEN" : geophone.fault==-2 ? "SHORT" : geophone.fault==1 ?  "NG" : "OK"
	);	
	f_write(&MyFile, str, strlen(str), (void *)&wbytes);
  f_close(&MyFile);
	
	GUI_SetColor(BLACK);
	GUI_SetBkColor(WHITE);
	GUI_SetFont(&GUI_FontHelvetica32);	
	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_BOTTOM);
	if(++settings.serialno>99999)
		settings.serialno = 1;
	widgetList[2].widgetInit(&widgetList[2]);
	widgetList[2].widgetDraw(&widgetList[2]);
}

static void test()
{
	int ret;
	int i;
	
	testflag=1;
	GUI_SetColor(WHITE);
	GUI_FillRect(0,240,479,799);
	if(settings.iteration==1)
	{
		ret = geotest();
		sprintf(widgetList[1].widgetTitle,"Retest");
		
		if(ret==0)
			beep(500);
		else
		{
			beep(100);
			HAL_Delay(100);
			beep(100);
			HAL_Delay(100);
			beep(100);
		}
		if(ret>=0)
		{
			widgetList[0].enable = 1;
			widgetList[0].active = 1;
			widgetList[1].active = 0;
			pTest.widgetSelected = 0;
		}
		
	}
	else
	{	
		settings.ldrate=0;
		settings.polarity=0;
		widgetList[1].enable = 0;
		widgetList[0].enable = 1;
		while(settings.iteration>0)
		{
			GUI_SetColor(WHITE);
			GUI_FillRect(300,240,479,799);
			sprintf(widgetList[0].widgetTitle,"%d",settings.iteration);
			widgetList[0].widgetDraw(&widgetList[0]);
			geotest();
			save();
			for(i=0;i<settings.timeinterval;i++)
			{
				if((GPIOA->IDR&0x7) == 0x2)
				{
					settings.iteration=0;
					break;
				}
				osDelay(1000);			
			}
			if(i<settings.timeinterval)
				break;
			settings.iteration = settings.iteration-1;
		}
		widgetList[0].enable = 0;
		widgetList[1].enable = 1;
		settings.iteration=1;
		sprintf(widgetList[0].widgetTitle,"Save");
		beep(500);
	}
	UITouchClear();
	widgetList[0].widgetDraw(&widgetList[0]);
	widgetList[1].widgetDraw(&widgetList[1]);
}

static void panaldraw()
{
	int COLOR;
	char str[20];
	struct GeoParam *geo = &geoparam[settings.paramnum];
	
	if(!testflag)
		return;
	
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
	
	GUI_SetColor(WHITE);
	if(geophone.fault<0)
	{
		sprintf(str,geophone.fault==-1 ? "OPEN" : "SHORT");
		GUI_SetBkColor(0x005a62ff);
		GUI_SetTextAlign(GUI_TA_HCENTER);
		GUI_DispStringAt("              ",356,260);
		GUI_SetTextAlign(GUI_TA_HCENTER);
		GUI_DispStringAt(str,356,260);
		return ;
	}
	
	sprintf(str,"%.1f",geophone.nois);
	GUI_SetBkColor(geophone.nois>10 ? 0x005a62ff : 0x005bc15b);
	GUI_SetTextAlign(GUI_TA_HCENTER);
	GUI_DispStringAt("              ",356,260);
	GUI_SetTextAlign(GUI_TA_HCENTER);
	GUI_DispStringAt(str,356,260);
	
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
	{
		if(geophone.polarity)
		{
			GUI_SetColor(WHITE);
			GUI_SetBkColor(0x002fbeff);
			GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_TOP);
			GUI_DispStringAt("              ",300,580);
			GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_TOP);
			GUI_DispStringAt("0",356,580);
		}
		else
		{
			sprintf(str,"%d",geophone.polarity);
			GUI_SetBkColor(geophone.polarity!=1 ? 0x005a62ff : 0x005bc15b);
			GUI_DispStringAt("              ",300,580);
			GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_TOP);
			GUI_DispStringAt(str,356,580);
		}
	}
	
	if(settings.ldrate)
	{
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

static void widgetInit(struct UIWidget * widget)
{
	switch(widget->widgetIndex)
	{
		case 2:
			sprintf(widget->widgetPtr,"%05d",settings.serialno);
			break;
		case 3:
			strcpy(widget->widgetPtr,settings.code);
			break;
		default:
			break;
	}
}

static struct UIWidget widgetList[4] =
{
	{0,0,0,{20,700,219,760},"Save",0,NULL,NULL,drawButton,save},
	{1,1,0,{260,700,459,760},"Test",0,NULL,NULL,drawButton,test},
	{2,1,0,{20,140,299,179},"No.",0,StringArray[0],widgetInit,drawBox,goSubSettings},
	{3,1,0,{20,190,299,229},"Code",0,StringArray[1],widgetInit,drawBox,goSubSettings},
};

static void pageInit(struct UIPage * page)
{
//	char str[40];
	
	sprintf(page->pageTitle,settings.filename);
	sprintf(page->widgetList[1].widgetTitle,"Test");
	panaldraw();
}
	
static void pageReturn(struct UIPage * page)
{
	page->widgetSelected = 1;
	page->widgetList[1].active = 1;
	page->widgetList[1].enable = 1;
	page->widgetList[0].enable = 0;
	testflag = 0;
	saveflag = 0;
	PageJump(&pSetup);
}

struct UIPage pTest = 
{
	"Test",
	4,//char widgetNum;
	-1,
	widgetList,//struct UIWidget * widgetList;
	pageInit,
	pageReturn,
	keyboardEvent,
	touchEvent,
};

