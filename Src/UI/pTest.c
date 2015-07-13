#include "ui.h"
#include "geotest.h"
#include "flash.h"

extern struct UIPage pConfig;
extern struct GeoParam geoparam[10];
extern struct Settings settings;
extern struct UIInfo UIInfo;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelvetica32;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontVerdana32;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelveticaNeueLT48;

static struct UIWidget widgetList[10];
struct UIPage pTest;

static void save()
{
	sprintf(widgetList[1].widgetTitle,"Test");
	widgetList[0].enable = 0;
	widgetList[0].active = 0;
	widgetList[1].active = 1;
	pTest.widgetSelected = 1;
	widgetList[0].widgetDraw(&widgetList[0]);
	widgetList[1].widgetDraw(&widgetList[1]);
}
static void test()
{
	GUI_SetColor(WHITE);
	GUI_SetBkColor(WHITE);
	GUI_FillRect(0,350,479,799);
	geotest();
	sprintf(widgetList[1].widgetTitle,"Retest");
	widgetList[0].enable = 1;
	widgetList[0].widgetDraw(&widgetList[0]);
	widgetList[1].widgetDraw(&widgetList[1]);
}

static struct UIWidget widgetList[10] =
{
	{0,1,0,{20,700,219,760},"Save",0,NULL,NULL,drawButton,save},
	{1,1,0,{260,700,459,760},"Test",0,NULL,NULL,drawButton,test},
};

static void pageInit(struct UIPage * page)
{
	char str[20];
	
	page->widgetSelected = 1;
	page->widgetList[1].active = 1;
	page->widgetList[1].enable = 1;
	page->widgetList[0].enable = 0;
	
	sprintf(page->widgetList[1].widgetTitle,"Test");
	GUI_SetColor(BLACK);
	GUI_SetBkColor(WHITE);
	GUI_SetFont(&GUI_FontHelvetica32);	
	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_TOP);
	
	sprintf(str,"File Name:%s",settings.filename);
	GUI_DispStringAt(str,0,140);
	sprintf(str,"Serial No.:%d",settings.serialno);
	GUI_DispStringAt(str,0,180);
	sprintf(str,"Geo Type:%s",geoparam[settings.paramnum].type);
	GUI_DispStringAt(str,0,220);
}
	
static void pageReturn(struct UIPage * page)
{
	PageJump(&pConfig);
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

