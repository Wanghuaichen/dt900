#include "ui.h"
#include "geotest.h"
#include <stdlib.h>

extern struct UIPage pKeyboard;
extern struct KBInfo kbInfo;
extern struct GeoParam geoparam[10];
extern struct Settings settings;
static char StringArray[5][20];
extern struct UIPage pSetup;
struct UIPage pString;
static struct UIWidget widgetList[5];

static void kbCallBack()
{
	int val;
	val = atoi(kbInfo.kbBuff);
	switch(pString.widgetSelected)
	{
		case 0:
			settings.series = val<1 ? 1 : val >99 ? 99 : val;
			break;
		case 1:
			settings.parallel = val<1 ? 1 : val >99 ? 99 : val;
			break;
		case 2:
			settings.lineR =val<0 ? 0 : val>9999 ? 9999 : val;
			break;
		case 3:
			settings.leadin = val<0 ? 0 : val>9999 ? 9999 : val;
			break;
		case 4:
			settings.interval = val<0 ? 0 : val>9999 ? 9999 : val;
			break;
		default:
			break;
	}
}

static void goSubSettings(struct UIWidget * widget)
{
	kbInfo.kbParent = &pString;
	strcpy(kbInfo.kbTitle,widget->widgetTitle);
	strcpy(kbInfo.kbBuff,widget->widgetPtr);
	kbInfo.kbCallBack = kbCallBack;
	kbInfo.strlength = 5;
	PageJump(&pKeyboard);
}


static void widgetInit(struct UIWidget * widget)
{
	switch(widget->widgetIndex)
	{
		case 0:
			sprintf(widget->widgetPtr,"%d",settings.series);
			break;
		case 1:
			sprintf(widget->widgetPtr,"%d",settings.parallel);
			break;
		case 2:
			sprintf(widget->widgetPtr,"%d",settings.lineR);
			break;
		case 3:
			sprintf(widget->widgetPtr,"%d",settings.leadin);
			break;
		case 4:
			sprintf(widget->widgetPtr,"%d",settings.interval);
			break;
		default:
			break;
	}
}


static void pageReturn(struct UIPage * page)
{
	PageJump(&pSetup);
}


static struct UIWidget widgetList[5] =
{
	{0,1,0,{0,120,479,179},"Series",0,StringArray[0],widgetInit,drawSLabel,goSubSettings},
	{1,1,0,{0,180,479,239},"Parallel",0,StringArray[1],widgetInit,drawSLabel,goSubSettings},
	{2,1,0,{0,240,479,299},"Line Resist(}/km)",0,StringArray[2],widgetInit,drawSLabel,goSubSettings},
	{3,1,0,{0,300,479,359},"Lead-in(m)",0,StringArray[3],widgetInit,drawSLabel,goSubSettings},
	{4,1,0,{0,360,479,419},"Iterval(m)",0,StringArray[4],widgetInit,drawSLabel,goSubSettings},
};

struct UIPage pString = 
{
	"String",
	5,//char widgetNum;
	-1,
	widgetList,//struct UIWidget * widgetList;
	NULL,
	pageReturn,
	keyboardEvent,
	touchEvent,
};

