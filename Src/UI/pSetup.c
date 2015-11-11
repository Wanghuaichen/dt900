#include "ui.h"
#include "geotest.h"
#include <stdlib.h>
#include "ostask.h"

extern struct UIPage pMain;
extern struct UIPage pKeyboard;
extern struct UIPage pGeoParam;
extern struct UIPage pTest;
extern struct UIPage pString;
extern struct UIPage pFlow;
extern struct UIPage pIteration;
extern struct KBInfo kbInfo;
extern struct GeoParam geoparam[20];
extern struct Settings settings;
extern struct UIInfo UIInfo;
static char StringArray[10][30];
struct UIPage pSetup;
static struct UIWidget widgetList[10];

static void kbCallBack()
{
	char str[20];
	int val;
	float val2;
	switch(pSetup.widgetSelected)
	{
		case 0:
			strcpy(settings.filename,kbInfo.kbBuff);
			if(settings.filename[0] == '\0')
				sprintf(settings.filename,"Test");
			break;
		case 3:
			val2 = (int)(atof(kbInfo.kbBuff)*10+0.1)/10.0;
			if(settings.units)
				val2 = (val2-32)/1.8;
			settings.temperature = val2>99 ? 99 : val2<-99 ? -99 : val2;
			reTemp();
			break;
		case 4:
			val = atoi(kbInfo.kbBuff);
			settings.shunt = val>999999 ? 999999 : val<0 ? 0 : val;
			break;
		case 8:
			val = atoi(kbInfo.kbBuff);
			settings.iteration = val>99999 ? 99999 : val<1 ? 1 : val;
			break;
		default:
			break;
	}
}

static void goSubSettings(struct UIWidget * widget)
{
	kbInfo.kbParent = &pSetup;
	if(widget->widgetIndex==4)
		sprintf(kbInfo.kbTitle,"Shunt");
	else
		strcpy(kbInfo.kbTitle,widget->widgetTitle);
	strcpy(kbInfo.kbBuff,widget->widgetPtr);
	kbInfo.kbCallBack = kbCallBack;
	switch(widget->widgetIndex)
	{
		case 0:
			kbInfo.strlength = 16;
			break;
		case 4:
			kbInfo.strlength = 8;
			break;
		default:
			kbInfo.strlength = 6;
			break;
	}
	PageJump(&pKeyboard);
}

static void goSubPage(struct UIWidget * widget)
{
	switch(widget->widgetIndex)
	{
		case 1:
			PageJump(&pGeoParam);
			break;
		case 5:
			PageJump(&pString);
			break;
		case 6:
			PageJump(&pFlow);
			break;
		case 8:
			PageJump(&pIteration);
			break;
		default:
			PageJump(&pTest);
			break;
	}
}


static void widgetInit(struct UIWidget * widget)
{
	switch(widget->widgetIndex)
	{
		case 0:
			strcpy(widget->widgetPtr,settings.filename);
			break;
		case 1:
			strcpy(widget->widgetPtr,geoparam[settings.paramnum].type);
			break;
		case 2:
			if(settings.sensormode == 0)
				sprintf(widget->widgetPtr,"OFF");
			else if(settings.sensormode == 1)
				sprintf(widget->widgetPtr,"INNER");
			else if(settings.sensormode == 2)
				sprintf(widget->widgetPtr,"OUTTER");
			else if(settings.sensormode == 3)
				sprintf(widget->widgetPtr,"MANUAL");
			break;
		case 3:
			widget->enable = settings.sensormode != 3 ? 0 : 1;
			if(settings.units)
			{
				sprintf(widget->widgetTitle,"Temperature(~F)");
				sprintf(widget->widgetPtr,"%g",(int)((settings.temperature*1.8+32)*10+0.1)/10.0);
			}
			else
			{
				sprintf(widget->widgetTitle,"Temperature(~C)");
				sprintf(widget->widgetPtr,"%g",(int)(settings.temperature*10+0.1)/10.0);
			}
			break;
		case 4:
			sprintf(widget->widgetPtr,"%d",settings.shunt);
			break;
		case 5:
			sprintf(widget->widgetPtr,"%d{%d %d/%d/%d",
			settings.series,settings.parallel,settings.lineR,settings.leadin,settings.interval);
			break;
		case 6:
			sprintf(widget->widgetPtr,"std");
			if(settings.ldrate)
				sprintf(widget->widgetPtr,"%s/ld:%d",widget->widgetPtr,settings.ldrate);
			if(settings.polarity)
				sprintf(widget->widgetPtr,"%s/pol",widget->widgetPtr);
			break;
		case 7:
			sprintf(widget->widgetPtr,settings.constant ? "Velocity" : "Excursion");
			break;
		case 8:
			if(settings.iteration>1)			
				sprintf(widget->widgetPtr,"%d/%d",settings.iteration,settings.timeinterval);
			else
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
	reTemp();
	widget->widgetInit(widget);
	widget->widgetDraw(widget);
	UITouchClear();
	pSetup.widgetList[3].widgetInit(&pSetup.widgetList[3]);
	pSetup.widgetList[3].widgetDraw(&pSetup.widgetList[3]);
}

static void constant(struct UIWidget * widget)
{
	settings.constant = settings.constant ? 0 : 1;
	widget->widgetInit(widget);
	widget->widgetDraw(widget);
}

static struct UIWidget widgetList[10] =
{
	{0,1,0,{0,120,479,179},"File Name",0,StringArray[0],widgetInit,drawSLabel,goSubSettings},
	{1,1,0,{0,180,479,239},"Geo Spec.",0,StringArray[2],widgetInit,drawSLabel,goSubPage},
	{2,1,0,{0,240,479,299},"Sensor Mode",0,StringArray[3],widgetInit,drawSLabel,sensormode},
	{3,0,0,{0,300,479,359},"Temperature",0,StringArray[4],widgetInit,drawSLabel,goSubSettings},
	{4,1,0,{0,360,479,419},"Shunt(})  0=none",0,StringArray[5],widgetInit,drawSLabel,goSubSettings},
	{5,1,0,{0,420,479,479},"String",0,StringArray[6],widgetInit,drawSLabel,goSubPage},
	{6,1,0,{0,480,479,539},"Test Flow",0,StringArray[7],widgetInit,drawSLabel,goSubPage},
	{7,1,0,{0,540,479,599},"@Constant",0,StringArray[8],widgetInit,drawSLabel,constant},
	{8,1,0,{0,600,479,659},"Multiple Tests",0,StringArray[9],widgetInit,drawSLabel,goSubPage},
	{9,1,0,{120,700,359,759},"Continue",0,NULL,NULL,drawButton,goSubPage},
};

struct UIPage pSetup = 
{
	"Test Setup",
	10,//char widgetNum;
	-1,
	widgetList,//struct UIWidget * widgetList;
	NULL,
	pageReturn,
	keyboardEvent,
	touchEvent,
};

