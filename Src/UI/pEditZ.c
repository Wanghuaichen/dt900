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

static char StringArray[4][20];
static struct UIWidget widgetList[4];
struct UIPage pEditZ;

static int impedance()
{
	float a,b,c,d,re,im;
	float w,w0,s,m,bt,r;
	char str[30];
	
	w = geoparam[pSettings.widgetSelected].DF*2*PI;
	w0 = geoparam[pSettings.widgetSelected].F*2*PI;
	s = geoparam[pSettings.widgetSelected].S;
	m = geoparam[pSettings.widgetSelected].M;
	bt = geoparam[pSettings.widgetSelected].B;
	r = geoparam[pSettings.widgetSelected].R;
	
	if(!(w&&w0&&s&&m&&bt&&r))
		return 0;
	
	a = w*s*s/m;
	b = w0*w0-w*w;
	c = 2*bt*w*w0;
	d = b*b+c*c;
	re = r+a*c/d;
	im = a*b/d;
	return (int)round(sqrt(re*re+im*im));
}

static void kbCallBack()
{
	char str[20];
	float val;
	switch(pEditZ.widgetSelected)
	{
		case 0:
			geoparam[pSettings.widgetSelected].Z = atoi(kbInfo.kbBuff);
			break;
		case 1:
			val = (int)(atof(kbInfo.kbBuff)*10+0.1)/1000.0;
			geoparam[pSettings.widgetSelected].Zp =  val<0 ? 0 : val >1 ? 1 : val;
			break;
		case 2:
			val = (int)(atof(kbInfo.kbBuff)*10+0.1)/1000.0;
			geoparam[pSettings.widgetSelected].Zn =  val<0 ? 0 : val >1 ? 1 : val;
			break;
		default:
			break;
	}
}

static void goSubSettings(struct UIWidget * widget)
{
	kbInfo.kbParent = &pEditZ;
	strcpy(kbInfo.kbTitle,widget->widgetTitle);
	strcpy(kbInfo.kbBuff,widget->widgetPtr);
	kbInfo.kbCallBack = kbCallBack;
	kbInfo.strlength = 5;
	PageJump(&pKeyboard);
}

static void widgetInit(struct UIWidget * widget)
{
	int val;
	switch(widget->widgetIndex)
	{
		case 0:
			sprintf(widget->widgetPtr,"%d",(int)geoparam[pSettings.widgetSelected].Z);
			break;
		case 1:
			sprintf(widget->widgetPtr,"%g",geoparam[pSettings.widgetSelected].Zp*100);
			break;
		case 2:
			sprintf(widget->widgetPtr,"%g",geoparam[pSettings.widgetSelected].Zn*100);
			break;
		case 3:
			val = impedance();
			if(val)
				sprintf(widget->widgetPtr,"%d",val);
			else
				sprintf(widget->widgetPtr,"--");
			break;
		default:
			break;
	}
}

static void pageReturn(struct UIPage * page)
{
	PageJump(&pSubSettings);
}

static struct UIWidget widgetList[4] =
{
	{0,1,0,{0,120,479,179},"Impedance(})",0,StringArray[0],widgetInit,drawSLabel,goSubSettings},
	{1,1,0,{0,180,479,239},"Z Max(+%)",0,StringArray[1],widgetInit,drawSLabel,goSubSettings},
	{2,1,0,{0,240,479,299},"Z Min(-%)",0,StringArray[2],widgetInit,drawSLabel,goSubSettings},
	{3,0,0,{0,300,479,359},"Reference",0,StringArray[3],widgetInit,drawSLabel,NULL},
};

struct UIPage pEditZ = 
{
	"Impedance",
	4,//char widgetNum;
	-1,
	widgetList,//struct UIWidget * widgetList;
	NULL,
	pageReturn,
	keyboardEvent,
	touchEvent,
};

