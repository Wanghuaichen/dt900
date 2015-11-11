#include "ui.h"
#include "geotest.h"
#include <stdlib.h>
#include <math.h>

extern struct UIPage pSettings;
extern struct UIPage pSubSettings;
extern struct UIPage pKeyboard;
extern struct KBInfo kbInfo;
extern struct GeoParam geoparam[20];
extern struct Settings settings;

static char StringArray[2][20];
static struct UIWidget widgetList[2];
struct UIPage pEditDrive;

static void kbCallBack()
{
	char str[20];
	float val;
	int val2;
	switch(pEditDrive.widgetSelected)
	{
		case 0:
			val2 = atoi(kbInfo.kbBuff);
		geoparam[pSettings.widgetSelected].DF = val2<0 ? 0 : val2;
			break;
		case 1:
			val = round(atof(kbInfo.kbBuff)*100)/100.0;
			geoparam[pSettings.widgetSelected].speed =  val<0 ? 0 : val >10 ? 10 : val;
			break;
		default:
			break;
	}
}

static void goSubSettings(struct UIWidget * widget)
{
	kbInfo.kbParent = &pEditDrive;
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
			sprintf(widget->widgetPtr,"%d",(int)geoparam[pSettings.widgetSelected].DF);
			break;
		case 1:
			sprintf(widget->widgetPtr,"%g",geoparam[pSettings.widgetSelected].speed);
			break;
		default:
			break;
	}
}

static void pageReturn(struct UIPage * page)
{
	PageJump(&pSubSettings);
}

static struct UIWidget widgetList[2] =
{
	{0,1,0,{0,120,479,179},"Frequency(Hz)",0,StringArray[0],widgetInit,drawSLabel,goSubSettings},
	{1,1,0,{0,180,479,239},"Velocity(\"/s)",0,StringArray[1],widgetInit,drawSLabel,goSubSettings},
};

struct UIPage pEditDrive = 
{
	"Drive",
	2,//char widgetNum;
	-1,
	widgetList,//struct UIWidget * widgetList;
	NULL,
	pageReturn,
	keyboardEvent,
	touchEvent,
};

