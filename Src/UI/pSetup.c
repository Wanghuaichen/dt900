#include "ui.h"
#include "geotest.h"
#include <stdlib.h>
#include "ostask.h"

extern struct UIPage pMain;
extern struct UIPage pKeyboard;
extern struct UIPage pGeoParam;
extern struct UIPage pTest;
extern struct KBInfo kbInfo;
extern struct GeoParam geoparam[10];
extern struct Settings settings;
extern struct UIInfo UIInfo;
static char StringArray[9][20];
struct UIPage pSetup;
static struct UIWidget widgetList[10];

static void kbCallBack()
{
	char str[20];
	int val;
	switch(pSetup.widgetSelected)
	{
		case 0:
			strcpy(settings.filename,kbInfo.kbBuff);
			if(settings.filename[0] == '\0')
				sprintf(settings.filename,"Test");
			break;
		case 1:
			val = atoi(kbInfo.kbBuff);
			settings.serialno = val<1 ? 1 : val;
			break;
		case 4:
			settings.temperature = (int)(atof(kbInfo.kbBuff)*10)/10.0;
			break;
		case 5:
			val = atoi(kbInfo.kbBuff);
			settings.shunt = val>9999 ? 9999 : val<0 ? 0 : val;
			if(settings.shunt>9999)
				settings.shunt = 9999;
		case 6:
			val = atoi(kbInfo.kbBuff);
			settings.strings = val>99 ? 99 : val<1 ? 1 : val;
			break;
		case 8:
			val = atoi(kbInfo.kbBuff);
			settings.iteration = val>999999 ? 999999 : val<1 ? 1 : val;
			break;
		default:
			break;
	}
	UIInfo.flagSettings = 1;
}

static void goSubSettings(struct UIWidget * widget)
{
	kbInfo.kbParent = &pSetup;
	strcpy(kbInfo.kbTitle,widget->widgetTitle);
	strcpy(kbInfo.kbBuff,widget->widgetPtr);
	kbInfo.kbCallBack = kbCallBack;
	PageJump(&pKeyboard);
}

static void goSubPage(struct UIWidget * widget)
{
	switch(widget->widgetIndex)
	{
		case 2:
			PageJump(&pGeoParam);
			break;
		default:
			PageJump(&pTest);
			break;
	}
	UIInfo.flagSettings = 1;
}


static void widgetInit(struct UIWidget * widget)
{
	switch(widget->widgetIndex)
	{
		case 0:
			strcpy(widget->widgetPtr,settings.filename);
			break;
		case 1:
			sprintf(widget->widgetPtr,"%04d",settings.serialno);
			break;
		case 2:
			strcpy(widget->widgetPtr,geoparam[settings.paramnum].type);
			break;
		case 3:
			if(settings.sensormode == 0)
				sprintf(widget->widgetPtr,"OFF");
			else if(settings.sensormode == 1)
				sprintf(widget->widgetPtr,"INNER");
			else if(settings.sensormode == 2)
				sprintf(widget->widgetPtr,"OUTTER");
			else if(settings.sensormode == 3)
				sprintf(widget->widgetPtr,"MANUAL");
			break;
		case 4:
			widget->enable = settings.sensormode != 3 ? 0 : 1;
			sprintf(widget->widgetPtr,"%g",settings.temperature);
			break;
		case 5:
			sprintf(widget->widgetPtr,"%d",settings.shunt);
			break;
		case 6:
			sprintf(widget->widgetPtr,"%d",settings.strings);
			break;
		case 7:
			sprintf(widget->widgetPtr,"std");
			break;
		case 8:
			sprintf(widget->widgetPtr,"%d",settings.iteration);
			break;
		default:
			break;
	}
}


static void pageReturn(struct UIPage * page)
{
	PageJump(&pMain);
}

static void sensormode(struct UIWidget * widget)
{
	settings.sensormode = (settings.sensormode+1)&0x3;
	widget->widgetInit(widget);
	widget->widgetDraw(widget);
	pSetup.widgetList[4].widgetInit(&pSetup.widgetList[4]);
	pSetup.widgetList[4].widgetDraw(&pSetup.widgetList[4]);
	UIInfo.flagSettings = 1;
	reTemp();
}

static struct UIWidget widgetList[10] =
{
	{0,1,0,{0,120,479,179},"File Name",0,StringArray[0],widgetInit,drawSLabel,goSubSettings},
	{1,1,0,{0,180,479,239},"Serial No.",0,StringArray[1],widgetInit,drawSLabel,goSubSettings},
	{2,1,0,{0,240,479,299},"Geo Spec.",0,StringArray[2],widgetInit,drawSLabel,goSubPage},
	{3,1,0,{0,300,479,359},"Sensor Mode",0,StringArray[3],widgetInit,drawSLabel,sensormode},
	{4,0,0,{0,360,479,419},"Temperature",0,StringArray[4],widgetInit,drawSLabel,goSubSettings},
	{5,1,0,{0,420,479,479},"Shunt(})",0,StringArray[5],widgetInit,drawSLabel,goSubSettings},
	{6,1,0,{0,480,479,539},"String No.",0,StringArray[6],widgetInit,drawSLabel,goSubSettings},
	{7,1,0,{0,540,479,599},"Test Flow",0,StringArray[7],widgetInit,drawSLabel,NULL},
	{8,1,0,{0,600,479,659},"Iteration",0,StringArray[8],widgetInit,drawSLabel,goSubSettings},
	{9,1,0,{120,720,359,779},"Continue",0,NULL,NULL,drawButton,goSubPage},
};

struct UIPage pSetup = 
{
	"Test",
	10,//char widgetNum;
	-1,
	widgetList,//struct UIWidget * widgetList;
	NULL,
	pageReturn,
	keyboardEvent,
	touchEvent,
};

