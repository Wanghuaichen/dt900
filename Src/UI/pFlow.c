#include "ui.h"
#include "geotest.h"
#include <stdlib.h>

extern struct UIPage pKeyboard;
extern struct KBInfo kbInfo;
extern struct Settings settings;
static char StringArray[2][20];
extern struct UIPage pSetup;
struct UIPage pFlow;
static struct UIWidget widgetList[2];

static void kbCallBack()
{
	int val = atoi(kbInfo.kbBuff);
	settings.ldrate = val<0 ? 0 : val>100 ? 100 : val;
}

static void goSubSettings(struct UIWidget * widget)
{
	kbInfo.kbParent = &pFlow;
	strcpy(kbInfo.kbTitle,widget->widgetTitle);
	strcpy(kbInfo.kbBuff,widget->widgetPtr);
	kbInfo.kbCallBack = kbCallBack;
	kbInfo.strlength = 4;
	PageJump(&pKeyboard);
}


static void widgetInit(struct UIWidget * widget)
{
	switch(widget->widgetIndex)
	{
		case 0:
			sprintf(widget->widgetPtr,"%d",settings.ldrate);
			break;
		case 1:
			sprintf(widget->widgetPtr,settings.polarity ? "ON" : "OFF");
			break;
		default:
			break;
	}
}


static void pageReturn(struct UIPage * page)
{
	PageJump(&pSetup);
}

static void polarity(struct UIWidget * widget)
{
	settings.polarity = settings.polarity ? 0 : 1;
	widget->widgetInit(widget);
	widget->widgetDraw(widget);
}

static struct UIWidget widgetList[2] =
{
	{0,1,0,{0,120,479,179},"Low Drive(%)",0,StringArray[0],widgetInit,drawSLabel,goSubSettings},
	{1,1,0,{0,180,479,239},"Polarity Test",0,StringArray[1],widgetInit,drawSLabel,polarity},
};

struct UIPage pFlow = 
{
	"Test Flow",
	2,//char widgetNum;
	-1,
	widgetList,//struct UIWidget * widgetList;
	NULL,
	pageReturn,
	keyboardEvent,
	touchEvent,
};

