#include "ui.h"
#include "geotest.h"

extern struct UIPage pSettings;
extern struct UIPage pKeyboard;
extern struct GeoParam geoparam[10];
extern struct Settings settings;

static char StringArray[10][20];

static void goSubSettings(struct UIWidget * widget)
{
//	KeyboardPage = pSubSettings;
//	strcpy(KeyboardBuffer,widget->
//	PageJump(&pKeyboard);
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
			sprintf(widget->widgetPtr,"%.1f",geoparam[pSettings.widgetSelected].F);
			break;
		case 3:
			sprintf(widget->widgetPtr,"%.3f",geoparam[pSettings.widgetSelected].B);
			break;
		case 4:
			sprintf(widget->widgetPtr,"%.1f",geoparam[pSettings.widgetSelected].S);
			break;
		case 5:
			sprintf(widget->widgetPtr,"%.3f",geoparam[pSettings.widgetSelected].D);
			break;
		case 6:
			sprintf(widget->widgetPtr,"%d",(int)geoparam[pSettings.widgetSelected].DF);
			break;
		case 7:
			sprintf(widget->widgetPtr,"%.1f",geoparam[pSettings.widgetSelected].X);
			break;
		case 8:
			sprintf(widget->widgetPtr,"%.1f",geoparam[pSettings.widgetSelected].M);
			break;
		case 9:
			sprintf(widget->widgetPtr,"%.1f",geoparam[pSettings.widgetSelected].T);
			break;
		default:
			break;
	}
}

static void del(struct UIWidget * widget)
{
	
}
static void pageInit()
{}
	
static struct UIWidget widgetList[11] =
{
	{0,1,0,{0,120,0,179},"Type",0,StringArray[0],widgetInit,drawSLabel,goSubSettings},
	{1,1,0,{0,180,0,239},"Resistence(Ohm)",0,StringArray[1],widgetInit,drawSLabel,goSubSettings},
	{2,1,0,{0,240,0,299},"Frequency(Hz)",0,StringArray[2],widgetInit,drawSLabel,goSubSettings},
	{3,1,0,{0,300,0,359},"Damping",0,StringArray[3],widgetInit,drawSLabel,goSubSettings},
	{4,1,0,{0,360,0,419},"Sensitivity(V/m/s)",0,StringArray[4],widgetInit,drawSLabel,goSubSettings},
	{5,1,0,{0,420,0,479},"Distortion(%)",0,StringArray[5],widgetInit,drawSLabel,goSubSettings},
	{6,1,0,{0,480,0,539},"F distortion(Hz)",0,StringArray[6],widgetInit,drawSLabel,goSubSettings},
	{7,1,0,{0,540,0,599},"Excurtion(mm)",0,StringArray[7],widgetInit,drawSLabel,goSubSettings},
	{8,1,0,{0,600,0,659},"Mass(g)",0,StringArray[8],widgetInit,drawSLabel,goSubSettings},
	{9,1,0,{0,660,0,719},"Spec. temp(~C)",0,StringArray[9],widgetInit,drawSLabel,goSubSettings},
	{10,1,0,{100,730,379,789},"Delete",0,NULL,NULL,drawButton,del},
};
	
struct UIPage pKeyboard = 
{
	"",
	NULL,//struct UIPage * parent;
	13,//char widgetNum;
	-1,
	widgetList,//struct UIWidget * widgetList;
	pageInit,
	keyboardEvent,
	touchEvent,
};

