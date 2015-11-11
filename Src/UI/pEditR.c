#include "ui.h"
#include "geotest.h"
#include <stdlib.h>

extern struct UIPage pSettings;
extern struct UIPage pSubSettings;
extern struct UIPage pKeyboard;
extern struct KBInfo kbInfo;
extern struct GeoParam geoparam[20];
extern struct Settings settings;

static char StringArray[3][20];
static struct UIWidget widgetList[3];
struct UIPage pEditR;

static void kbCallBack()
{
	char str[20];
	float val;
	switch(pEditR.widgetSelected)
	{
		case 0:
			geoparam[pSettings.widgetSelected].R = atoi(kbInfo.kbBuff);
			break;
		case 1:
			val = (int)(atof(kbInfo.kbBuff)*10+0.1)/1000.0;
			geoparam[pSettings.widgetSelected].Rp =  val<0 ? 0 : val >1 ? 1 : val;
			break;
		case 2:
			val = (int)(atof(kbInfo.kbBuff)*10+0.1)/1000.0;
			geoparam[pSettings.widgetSelected].Rn =  val<0 ? 0 : val >1 ? 1 : val;
			break;
		default:
			break;
	}
}

static void goSubSettings(struct UIWidget * widget)
{
	kbInfo.kbParent = &pEditR;
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
			sprintf(widget->widgetPtr,"%d",(int)geoparam[pSettings.widgetSelected].R);
			break;
		case 1:
			sprintf(widget->widgetPtr,"%g",geoparam[pSettings.widgetSelected].Rp*100);
			break;
		case 2:
			sprintf(widget->widgetPtr,"%g",geoparam[pSettings.widgetSelected].Rn*100);
			break;
		default:
			break;
	}
}

static void pageReturn(struct UIPage * page)
{
	PageJump(&pSubSettings);
}

static struct UIWidget widgetList[3] =
{
	{0,1,0,{0,120,479,179},"Resistence(})",0,StringArray[0],widgetInit,drawSLabel,goSubSettings},
	{1,1,0,{0,180,479,239},"R Max(+%)",0,StringArray[1],widgetInit,drawSLabel,goSubSettings},
	{2,1,0,{0,240,479,299},"R Min(-%)",0,StringArray[2],widgetInit,drawSLabel,goSubSettings},
};

struct UIPage pEditR = 
{
	"Resistence",
	3,//char widgetNum;
	-1,
	widgetList,//struct UIWidget * widgetList;
	NULL,
	pageReturn,
	keyboardEvent,
	touchEvent,
};

