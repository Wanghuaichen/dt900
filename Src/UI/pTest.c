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

static struct UIWidget widgetList[2];
struct UIPage pTest;

static void header(FIL* pMyFile,char* str,UINT* pwbytes)
{
	if(UIInfo.flagSettings>1)
		return;
	sprintf(str,"*****************************************************************************************************************************\r\n");
	f_write(pMyFile, str, strlen(str),pwbytes);
	sprintf(str,"Device\tDT900\tPID\t%s\tSetup\t%s\r\n",UIInfo.uid,geoparam[settings.paramnum].type);
	f_write(pMyFile, str, strlen(str),pwbytes);
	sprintf(str,"Shunt\t%d\tSeries\t%d\tParallel\t%d\r\n",settings.shunt,settings.series,settings.parallel);
	f_write(pMyFile, str, strlen(str),pwbytes);
//	sprintf(str,"Line Resistance\t%d\tLead-in\t%d\tInterval\t%d\r\n",settings.lineR,settings.leadin,settings.interval);
//	f_write(pMyFile, str, strlen(str),pwbytes);
	sprintf(str,"Temp. Correct.\t%s\t@Constant\t%s\r\n\r\n",settings.sensormode==0 ? "OFF" : settings.sensormode==1 ? "INNER" : settings.sensormode==2 ? "OUTTER" : "MANUAL",settings.constant ? "VELOCITY" : "EXCURTION");
	f_write(pMyFile, str, strlen(str),pwbytes);
	sprintf(str,"No.\tDate\tTime\tTemperature(C)\tNoise(mV)\tLeakage(Mohm)\tResistance(ohm)\tFrequency(Hz)\tDamping(-)\tSensitivity(V/m/s)\tDistortion(%%)\tImpedance(ohm)\tPolarity\tDescription\r\n");
	f_write(pMyFile, str, strlen(str),pwbytes);
}
	
static void save()
{
	FIL MyFile;
	char str[200];
	UINT wbytes;
	
	sprintf(widgetList[1].widgetTitle,"Test");
	widgetList[0].enable = 0;
	widgetList[0].active = 0;
	widgetList[1].active = 1;
	pTest.widgetSelected = 1;
	widgetList[0].widgetDraw(&widgetList[0]);
	widgetList[1].widgetDraw(&widgetList[1]);
dbg("save 0");	
	sprintf(str,"0:%s.geo",settings.filename);
dbg("save 1");	
	if(f_open(&MyFile,(TCHAR *)str,FA_OPEN_ALWAYS | FA_WRITE) != FR_OK)
		return;
dbg("save 2");	
  f_lseek(&MyFile,MyFile.fsize); 
dbg("save 3");
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
dbg("save 4");		
	f_write(&MyFile, str, strlen(str), (void *)&wbytes);
dbg("save 5");	
  f_close(&MyFile);
dbg("save 6");	
	
	GUI_SetColor(BLACK);
	GUI_SetBkColor(WHITE);
	GUI_SetFont(&GUI_FontHelvetica32);	
	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_BOTTOM);
	if(++settings.serialno>99999)
		settings.serialno = 1;
	sprintf(str,"No.:%05d",settings.serialno);
	UIInfo.flagSettings |= 0x2;
	GUI_DispStringAt(str,0,210);
}

static void test()
{
	int ret;
	GUI_SetColor(WHITE);
	GUI_FillRect(0,220,479,799);
	widgetList[0].enable = 0;
	
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
			widgetList[0].enable = 1;
	}
	else
	{	
		settings.ldrate=0;
		settings.polarity=0;
		widgetList[1].enable = 0;
		widgetList[0].enable = 1;
		sprintf(widgetList[0].widgetTitle,"%d",settings.iteration);
		widgetList[0].widgetDraw(&widgetList[0]);
		while(settings.iteration-->0)
		{
			GUI_SetColor(WHITE);
			GUI_FillRect(300,220,479,799);
			sprintf(widgetList[0].widgetTitle,"%d",settings.iteration);
			geotest();
			save();
			if((GPIOA->IDR&0x7) == 0x2)
			{
				settings.iteration=0;
				break;
			}
			osDelay(3000);
		}
		widgetList[0].enable = 0;
		widgetList[1].enable = 1;
		settings.iteration=1;
		sprintf(widgetList[0].widgetTitle,"Save");
		beep(500);
	}
	widgetList[0].widgetDraw(&widgetList[0]);
	widgetList[1].widgetDraw(&widgetList[1]);
	UITouchClear();
}

static struct UIWidget widgetList[2] =
{
	{0,1,0,{20,700,219,760},"Save",0,NULL,NULL,drawButton,save},
	{1,1,0,{260,700,459,760},"Test",0,NULL,NULL,drawButton,test},
};

static void pageInit(struct UIPage * page)
{
	char str[40];
	
	sprintf(page->pageTitle,geoparam[settings.paramnum].type);
	
	page->widgetSelected = 1;
	page->widgetList[1].active = 1;
	page->widgetList[1].enable = 1;
	page->widgetList[0].enable = 0;
	
	sprintf(page->widgetList[1].widgetTitle,"Test");
	GUI_SetColor(BLACK);
	GUI_SetBkColor(WHITE);
	GUI_SetFont(&GUI_FontHelvetica32);	
	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_BOTTOM);
	
	sprintf(str,"File:%s",settings.filename);
	GUI_DispStringAt(str,0,170);
	sprintf(str,"No.:%05d",settings.serialno);
	GUI_DispStringAt(str,0,210);
	
	if(settings.iteration>1)
	{
		sprintf(page->widgetList[0].widgetTitle,"%d",settings.iteration);
	}
}
	
static void pageReturn(struct UIPage * page)
{
	PageJump(&pSetup);
}

struct UIPage pTest = 
{
	"Test",
	2,//char widgetNum;
	-1,
	widgetList,//struct UIWidget * widgetList;
	pageInit,
	pageReturn,
	keyboardEvent,
	touchEvent,
};

