#include "ui.h"
#include "geotest.h"

extern struct UIPage pKeyboard;
extern struct KBInfo kbInfo;
extern struct UIPage pSetup;
extern struct Settings settings;
extern struct UIInfo UIInfo;
static char StringArray[2][20];
static struct UIWidget widgetList[2];

struct UIPage pIteration;
	
static void kbCallBack()
{
	char str[20];
	int val;
	switch(pIteration.widgetSelected)
	{
		case 0:
			val = atoi(kbInfo.kbBuff);
			settings.iteration = val>99999 ? 99999 : val<1 ? 1 : val;
			break;
		case 1:
			val = atoi(kbInfo.kbBuff);
			settings.timeinterval = val<1 ? 1 : val>999 ? 999 : val;
			break;
		default:
			break;
	}
}

static void goSubSettings(struct UIWidget * widget)
{
	kbInfo.kbParent = &pIteration;
	strcpy(kbInfo.kbTitle,widget->widgetTitle);
	strcpy(kbInfo.kbBuff,widget->widgetPtr);
	kbInfo.kbCallBack = kbCallBack;
	switch(widget->widgetIndex)
	{
		case 0:
			kbInfo.strlength = 6;
			break;
		default:
			kbInfo.strlength = 4;
			break;
	}
	PageJump(&pKeyboard);
}

static void widgetInit(struct UIWidget * widget)
{
	switch(widget->widgetIndex)
	{
		case 0:
			sprintf(widget->widgetPtr,"%d",settings.iteration);
			break;
		case 1:
			sprintf(widget->widgetPtr,"%d",settings.timeinterval);
			break;
		default:
			break;
	}
}


static void pageReturn(struct UIPage * page)
{
	PageJump(&pSetup);
}

static struct UIWidget widgetList[2] =
{
	{0,1,0,{0,120,479,179},"Repeat",0,StringArray[0],widgetInit,drawSLabel,goSubSettings},
	{1,1,0,{0,180,479,239},"Time Interval(Secs)",0,StringArray[1],widgetInit,drawSLabel,goSubSettings},
};

struct UIPage pIteration = 
{
	"Multiple Tests",
	2,//char widgetNum;
	-1,
	widgetList,//struct UIWidget * widgetList;
	NULL,
	pageReturn,
	keyboardEvent,
	touchEvent,
};

