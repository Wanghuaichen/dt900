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

static char StringArray[3][20];
static struct UIWidget widgetList[3];
struct UIPage pEditS;

static void kbCallBack()
{
	char str[20];
	float val;
	switch(pEditS.widgetSelected)
	{
		case 0:
			if(settings.units)
			{
				val = round(atof(kbInfo.kbBuff)*1000)/1000.0;
				val = round(val*39.37*10)/10.0;
			}
			else
				val = round(atof(kbInfo.kbBuff)*10)/10.0;
			geoparam[pSettings.widgetSelected].S = val<0 ? 0 : val >999 ? 999 : val;
			break;
		case 1:
			val = round(atof(kbInfo.kbBuff)*10)/1000.0;
			geoparam[pSettings.widgetSelected].Sp = val<0 ? 0 : val >1 ? 1 : val;
			break;
		case 2:
			val = round(atof(kbInfo.kbBuff)*10)/1000.0;
			geoparam[pSettings.widgetSelected].Sn = val<0 ? 0 : val >1 ? 1 : val;
			break;
		default:
			break;
	}
}

static void goSubSettings(struct UIWidget * widget)
{
	kbInfo.kbParent = &pEditS;
	strcpy(kbInfo.kbTitle,widget->widgetTitle);
	strcpy(kbInfo.kbBuff,widget->widgetPtr);
	kbInfo.kbCallBack = kbCallBack;
	kbInfo.strlength = widget->widgetIndex ? 5 : 6;
	PageJump(&pKeyboard);
}

static void widgetInit(struct UIWidget * widget)
{
	switch(widget->widgetIndex)
	{
		case 0:
			if(settings.units)
			{
				sprintf(widget->widgetTitle,"Sensitivity(V/\"/s)");
				sprintf(widget->widgetPtr,"%g",round(geoparam[pSettings.widgetSelected].S/39.37*1000)/1000.0);
			}
			else
			{
				sprintf(widget->widgetTitle,"Sensitivity(V/m/s)");
				sprintf(widget->widgetPtr,"%g",geoparam[pSettings.widgetSelected].S);
			}
			break;
		case 1:
			sprintf(widget->widgetPtr,"%g",geoparam[pSettings.widgetSelected].Sp*100);
			break;
		case 2:
			sprintf(widget->widgetPtr,"%g",geoparam[pSettings.widgetSelected].Sn*100);
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
	{0,1,0,{0,120,479,179},"",0,StringArray[0],widgetInit,drawSLabel,goSubSettings},
	{1,1,0,{0,180,479,239},"S Max(+%)",0,StringArray[1],widgetInit,drawSLabel,goSubSettings},
	{2,1,0,{0,240,479,299},"S Min(-%)",0,StringArray[2],widgetInit,drawSLabel,goSubSettings},
};

struct UIPage pEditS = 
{
	"Sensitivity",
	3,//char widgetNum;
	-1,
	widgetList,//struct UIWidget * widgetList;
	NULL,
	pageReturn,
	keyboardEvent,
	touchEvent,
};

