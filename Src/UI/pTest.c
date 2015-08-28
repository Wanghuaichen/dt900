#include "ui.h"
#include "geotest.h"
#include "flash.h"
#include "ff_gen_drv.h"
#include "rtc.h"
#include "ostask.h"

extern struct UIPage pSetup;
extern struct GeoParam geoparam[10];
extern struct Settings settings;
extern struct Geophone geophone;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelvetica32;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontVerdana32;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelveticaNeueLT48;

static struct UIWidget widgetList[2];
struct UIPage pTest;

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
  sprintf(str,"%06d\t%s\t%02d-%02d-%02d\t%02d:%02d:%02d\t%.1f\t%.2f\t%d\t%.2f\t%5.3f\t%.1f\t%.3f\t%d\r\n",
	settings.serialno,
	geoparam[settings.paramnum].type,
	rtcDate.Year,rtcDate.Month,rtcDate.Date,rtcTime.Hours, rtcTime.Minutes, rtcTime.Seconds,
	geophone.temp,
	geophone.nois,
	(int)geophone.resi,
	geophone.freq,
	geophone.damp,
	geophone.sens,
	geophone.dist,
	(int)geophone.impe
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
		sprintf(str,"No.:%05d",++settings.serialno);
		GUI_DispStringAt(str,0,210);
}

static void test()
{
//	if(settings.iteration==1)
//	{
//		geotest();
//		sprintf(widgetList[1].widgetTitle,"Retest");
//		widgetList[0].enable = 1;
//	}
//	else
//	{	
//		settings.ldrate=0;
//		settings.polarity=0;
//		widgetList[1].enable = 0;
//		while(settings.iteration-->0)
//		{
//			sprintf(widgetList[0].widgetTitle,"%d",settings.iteration);
//			//GUI_SetColor(WHITE);
//			//GUI_FillRect(0,350,479,799);
//			geotest();
//			save();
//			if((GPIOA->IDR&0x7) == 0x2)
//			{
//				settings.iteration=0;
//				break;
//			}
//			osDelay(3000);
//		}
//		widgetList[1].enable = 1;
//		settings.iteration=1;
//		sprintf(widgetList[0].widgetTitle,"Save");
//	}
//	widgetList[0].widgetDraw(&widgetList[0]);
//	widgetList[1].widgetDraw(&widgetList[1]);
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

