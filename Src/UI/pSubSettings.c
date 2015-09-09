#include "ui.h"
#include "geotest.h"
#include <stdlib.h>
#include <math.h>

extern struct UIPage pSettings;
extern struct UIPage pKeyboard;
extern struct UIPage pEditR;
extern struct UIPage pEditF;
extern struct UIPage pEditD;
extern struct UIPage pEditS;
extern struct KBInfo kbInfo;
extern struct GeoParam geoparam[10];
extern struct Settings settings;

static char StringArray[10][20];
struct UIPage pSubSettings;
static struct UIWidget widgetList[11];

static void impedence()
{
	float a,b,c,d,re,im;
	float w,w0,s,m,bt,r;
	
	w = geoparam[pSettings.widgetSelected].DF*2*PI;
	w0 = geoparam[pSettings.widgetSelected].F*2*PI;
	s = geoparam[pSettings.widgetSelected].S;
	m = geoparam[pSettings.widgetSelected].M;
	bt = geoparam[pSettings.widgetSelected].B;
	r = geoparam[pSettings.widgetSelected].R;
	
	a = w*s*s/m;
	b = w0*w0-w*w;
	c = 2*bt*w*w0;
	d = b*b+c*c;
	re = r+a*c/d;
	im = a*b/d;
	geoparam[pSettings.widgetSelected].Z = round(sqrt(re*re+im*im));
	
	GUI_DispDecAt((int)geoparam[pSettings.widgetSelected].Z,0,750,6);
}

static void kbCallBack()
{
	char str[20];
	switch(pSubSettings.widgetSelected)
	{
		case 0:
			strcpy(geoparam[pSettings.widgetSelected].type,kbInfo.kbBuff);
			break;
//		case 1:
//			geoparam[pSettings.widgetSelected].R = atoi(kbInfo.kbBuff);
//			break;
//		case 2:
//			geoparam[pSettings.widgetSelected].F = (int)(atof(kbInfo.kbBuff)*10)/10.0;
//			break;
//		case 3:
//			geoparam[pSettings.widgetSelected].B = (int)(atof(kbInfo.kbBuff)*1000)/1000.0;
//			break;
//		case 4:
//			geoparam[pSettings.widgetSelected].S = (int)(atof(kbInfo.kbBuff)*10)/10.0;
//			break;
		case 5:
			geoparam[pSettings.widgetSelected].D = (int)(atof(kbInfo.kbBuff)*1000)/1000.0;
			break;
		case 6:
			geoparam[pSettings.widgetSelected].DF = atoi(kbInfo.kbBuff);
			break;
		case 7:
			geoparam[pSettings.widgetSelected].X = (int)(atof(kbInfo.kbBuff)*100)/100000.0;
			break;
		case 8:
			geoparam[pSettings.widgetSelected].M = (int)(atof(kbInfo.kbBuff)*100)/100000.0;
			break;
		case 9:
			geoparam[pSettings.widgetSelected].T = (int)(atof(kbInfo.kbBuff)*10)/10.0;
			break;
		default:
			break;
	}
}

static void goSubSettings(struct UIWidget * widget)
{
	kbInfo.kbParent = &pSubSettings;
	strcpy(kbInfo.kbTitle,widget->widgetTitle);
	strcpy(kbInfo.kbBuff,widget->widgetPtr);
	kbInfo.kbCallBack = kbCallBack;
	PageJump(&pKeyboard);
}

static void goSubPage(struct UIWidget * widget)
{
	switch(widget->widgetIndex)
	{
		case 1:
			PageJump(&pEditR);
			break;
		case 2:
			PageJump(&pEditF);
			break;
		case 3:
			PageJump(&pEditD);
			break;
		case 4:
			PageJump(&pEditS);
			break;
		default:
			break;
	}
}


static void widgetInit(struct UIWidget * widget)
{
	switch(widget->widgetIndex)
	{
		case 0:
			strcpy(widget->widgetPtr,geoparam[pSettings.widgetSelected].type);
			break;
		case 1:
			sprintf(widget->widgetPtr,"%d",(int)geoparam[pSettings.widgetSelected].R);
			break;
		case 2:
			sprintf(widget->widgetPtr,"%g",geoparam[pSettings.widgetSelected].F);
			break;
		case 3:
			sprintf(widget->widgetPtr,"%g",geoparam[pSettings.widgetSelected].B);
			break;
		case 4:
			sprintf(widget->widgetPtr,"%g",geoparam[pSettings.widgetSelected].S);
			break;
		case 5:
			sprintf(widget->widgetPtr,"%g",geoparam[pSettings.widgetSelected].D);
			break;
		case 6:
			sprintf(widget->widgetPtr,"%d",(int)geoparam[pSettings.widgetSelected].DF);
			break;
		case 7:
			sprintf(widget->widgetPtr,"%g",geoparam[pSettings.widgetSelected].X*1000);
			break;
		case 8:
			sprintf(widget->widgetPtr,"%g",geoparam[pSettings.widgetSelected].M*1000);
			break;
		case 9:
			sprintf(widget->widgetPtr,"%g",geoparam[pSettings.widgetSelected].T);
			impedence();
			break;
		default:
			break;
	}
}

static void del(struct UIWidget * widget)
{
	memcpy(&geoparam[pSettings.widgetSelected],&geoparam[pSettings.widgetSelected+1],sizeof(struct GeoParam)*(10-1-pSettings.widgetSelected));
	if(settings.paramnum>0 && settings.paramnum>=pSettings.widgetSelected)
		settings.paramnum--;
	if(pSettings.widgetSelected<settings.totalparam)
		settings.totalparam--;
	widget->active=0;
	pSubSettings.widgetSelected = -1;
	PageJump(&pSettings);
}

static void pageReturn(struct UIPage * page)
{
	impedence();
	if(geoparam[pSettings.widgetSelected].type[0] == 0)
				strcpy(geoparam[pSettings.widgetSelected].type,"Empty");
	if(pSettings.widgetSelected==settings.totalparam)
		settings.totalparam++;
	PageJump(&pSettings);
}

static struct UIWidget widgetList[11] =
{
	{0,1,0,{0,120,479,179},"Type",0,StringArray[0],widgetInit,drawSLabel,goSubSettings},
	{1,1,0,{0,180,479,239},"Resistence(})",0,StringArray[1],widgetInit,drawSLabel,goSubPage},
	{2,1,0,{0,240,479,299},"Frequency(Hz)",0,StringArray[2],widgetInit,drawSLabel,goSubPage},
	{3,1,0,{0,300,479,359},"Damping",0,StringArray[3],widgetInit,drawSLabel,goSubPage},
	{4,1,0,{0,360,479,419},"Sensitivity(V/m/s)",0,StringArray[4],widgetInit,drawSLabel,goSubPage},
	{5,1,0,{0,420,479,479},"Distortion(%)",0,StringArray[5],widgetInit,drawSLabel,goSubSettings},
	{6,1,0,{0,480,479,539},"F distortion(Hz)",0,StringArray[6],widgetInit,drawSLabel,goSubSettings},
	{7,1,0,{0,540,479,599},"Excurtion(mm)",0,StringArray[7],widgetInit,drawSLabel,goSubSettings},
	{8,1,0,{0,600,479,659},"Mass(g)",0,StringArray[8],widgetInit,drawSLabel,goSubSettings},
	{9,1,0,{0,660,479,719},"Spec. temp(~C)",0,StringArray[9],widgetInit,drawSLabel,goSubSettings},
	{10,1,0,{120,740,359,779},"Delete",0,NULL,NULL,drawButton,del},
};

struct UIPage pSubSettings = 
{
	"Geophone",
	11,//char widgetNum;
	-1,
	widgetList,//struct UIWidget * widgetList;
	NULL,
	pageReturn,
	keyboardEvent,
	touchEvent,
};

