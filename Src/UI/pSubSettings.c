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
extern struct UIPage pEditZ;
extern struct UIPage pEditDrive;
extern struct KBInfo kbInfo;
extern struct GeoParam geoparam[20];
extern struct Settings settings;
extern int pageindex;

static char StringArray[11][20];
struct UIPage pSubSettings;
static struct UIWidget widgetList[12];

static void kbCallBack()
{
	char str[20];
	int index =pSettings.widgetSelected+10*pageindex;
	switch(pSubSettings.widgetSelected)
	{
		case 0:
			strcpy(geoparam[index].type,kbInfo.kbBuff);
			break;
		case 5:
			geoparam[index].D = round(atof(kbInfo.kbBuff)*1000)/1000.0;
			break;
		case 8:
			if(settings.units)
				geoparam[index].X = round(atof(kbInfo.kbBuff)*25.4*1000)/1000000.0;
			else
				geoparam[index].X = round(atof(kbInfo.kbBuff)*100)/100000.0;
			break;
		case 9:
			if(settings.units)
				geoparam[index].M = round(atof(kbInfo.kbBuff)*28.349523*100)/100000.0;
			else
				geoparam[index].M = round(atof(kbInfo.kbBuff)*100)/100000.0;
			break;
		case 10:
			if(settings.units)
				geoparam[index].T = round((atof(kbInfo.kbBuff)-32)/1.8*10)/10.0;
			else
				geoparam[index].T = round(atof(kbInfo.kbBuff)*10)/10.0;
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
	switch(widget->widgetIndex)
	{
		case 0:
			kbInfo.strlength = 16;
			break;
		case 5:
			kbInfo.strlength = 6;
			break;
		case 8:
			kbInfo.strlength = 6;
			break;
		case 9:
			kbInfo.strlength = 6;
			break;
		case 10:
			kbInfo.strlength = 6;
			break;
		default:
			kbInfo.strlength = 5;
			break;
	}
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
		case 6:
			PageJump(&pEditZ);
			break;
		case 7:
			PageJump(&pEditDrive);
			break;
		default:
			break;
	}
}


static void widgetInit(struct UIWidget * widget)
{
	int index =pSettings.widgetSelected+10*pageindex;
	switch(widget->widgetIndex)
	{
		case 0:
			strcpy(widget->widgetPtr,geoparam[index].type);
			break;
		case 1:
			sprintf(widget->widgetPtr,"%d",(int)geoparam[index].R);
			break;
		case 2:
			sprintf(widget->widgetPtr,"%g",geoparam[index].F);
			break;
		case 3:
			sprintf(widget->widgetPtr,"%g",geoparam[index].B);
			break;
		case 4:
			if(settings.units)
			{
				sprintf(widget->widgetTitle,"Sensitivity(V/\"/s)");
				sprintf(widget->widgetPtr,"%g",round(geoparam[index].S/1000*25.4*1000)/1000.0);
			}
			else
			{
				sprintf(widget->widgetTitle,"Sensitivity(V/m/s)");
				sprintf(widget->widgetPtr,"%g",geoparam[index].S);
			}
			break;
		case 5:
			sprintf(widget->widgetPtr,"%g",geoparam[index].D);
			break;
		case 6:
			sprintf(widget->widgetPtr,"%g",geoparam[index].Z);
			break;
		case 7:
			sprintf(widget->widgetPtr,"%d/%g",(int)geoparam[index].DF,geoparam[index].speed);
			break;
		case 8:
			if(settings.units)
			{
				sprintf(widget->widgetTitle,"Excursion(\")");
				sprintf(widget->widgetPtr,"%g",round(geoparam[index].X*1000/25.4*1000)/1000.0);
			}
			else
			{
				sprintf(widget->widgetTitle,"Excursion(mm)");
				sprintf(widget->widgetPtr,"%g",geoparam[index].X*1000);
			}
			break;
		case 9:
			if(settings.units)
			{
				sprintf(widget->widgetTitle,"Mass(oz)");
				sprintf(widget->widgetPtr,"%g",round(geoparam[index].M*1000*0.03527*1000)/1000.0);
			}
			else
			{
				sprintf(widget->widgetTitle,"Mass(g)");
				sprintf(widget->widgetPtr,"%g",geoparam[index].M*1000);
			}
			break;
		case 10:
			if(settings.units)
			{
				sprintf(widget->widgetTitle,"Spec. temp(~F)");
				sprintf(widget->widgetPtr,"%g",round((geoparam[index].T*1.8+32)*10)/10.0);
			}
			else
			{
				sprintf(widget->widgetTitle,"Spec. temp(~C)");
				sprintf(widget->widgetPtr,"%g",geoparam[index].T);
			}
			break;
		default:
			break;
	}
}

static void del(struct UIWidget * widget)
{
	int index =pSettings.widgetSelected+10*pageindex;
	memcpy(&geoparam[index],&geoparam[index+1],sizeof(struct GeoParam)*(20-1-index));
	if(settings.paramnum>0 && settings.paramnum>=index)
		settings.paramnum--;
	if(index<settings.totalparam)
		settings.totalparam--;
	widget->active=0;
	pSubSettings.widgetSelected = -1;
	PageJump(&pSettings);
}

static void pageReturn(struct UIPage * page)
{
	int index =pSettings.widgetSelected+10*pageindex;
	if(geoparam[index].type[0] == 0)
				strcpy(geoparam[index].type,"Empty");
	if(index==settings.totalparam)
		settings.totalparam++;
	PageJump(&pSettings);
}

static struct UIWidget widgetList[12] =
{
	{0,1,0,{0,120,479,175},"Type",0,StringArray[0],widgetInit,drawSLabel,goSubSettings},
	{1,1,0,{0,176,479,232},"Resistence(})",0,StringArray[1],widgetInit,drawSLabel,goSubPage},
	{2,1,0,{0,233,479,287},"Frequency(Hz)",0,StringArray[2],widgetInit,drawSLabel,goSubPage},
	{3,1,0,{0,288,479,343},"Damping",0,StringArray[3],widgetInit,drawSLabel,goSubPage},
	{4,1,0,{0,344,479,399},"Sensitivity(V/m/s)",0,StringArray[4],widgetInit,drawSLabel,goSubPage},
	{5,1,0,{0,400,479,455},"Distortion(%)",0,StringArray[5],widgetInit,drawSLabel,goSubSettings},
	{6,1,0,{0,456,479,511},"Impedance(})",0,StringArray[6],widgetInit,drawSLabel,goSubPage},
	{7,1,0,{0,512,479,567},"Drive",0,StringArray[7],widgetInit,drawSLabel,goSubPage},
	{8,1,0,{0,568,479,623},"Excursion(mm)",0,StringArray[8],widgetInit,drawSLabel,goSubSettings},
	{9,1,0,{0,624,479,679},"Mass(g)",0,StringArray[9],widgetInit,drawSLabel,goSubSettings},
	{10,1,0,{0,680,479,735},"Spec. temp(~C)",0,StringArray[10],widgetInit,drawSLabel,goSubSettings},
	{11,1,0,{160,745,319,790},"Delete",0,NULL,NULL,drawButton,del},
};

struct UIPage pSubSettings = 
{
	"Geophone",
	12,//char widgetNum;
	-1,
	widgetList,//struct UIWidget * widgetList;
	NULL,
	pageReturn,
	keyboardEvent,
	touchEvent,
};

