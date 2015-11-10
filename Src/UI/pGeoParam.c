#include "ui.h"
#include "geotest.h"
#include "flash.h"

extern struct GeoParam geoparam[10];
extern struct Settings settings;
extern struct UIInfo UIInfo;
extern struct UIPage pSetup;

static void selected(struct UIWidget * widget)
{
	settings.paramnum = widget->widgetIndex;
	PageJump(&pSetup);
}
	
static void widgetInit(struct UIWidget * widget)
{
	if(widget->widgetIndex < settings.totalparam)
	{
		strcpy(widget->widgetTitle,geoparam[widget->widgetIndex].type);
		widget->enable = 1;
	}
	else
		widget->enable = 0;
}

static struct UIWidget widgetList[10] =
{
	{0,0,0,{0,120,479,179},"",0,NULL,widgetInit,drawLabel,selected},
	{1,0,0,{0,180,479,239},"",0,NULL,widgetInit,drawLabel,selected},
	{2,0,0,{0,240,479,299},"",0,NULL,widgetInit,drawLabel,selected},
	{3,0,0,{0,300,479,359},"",0,NULL,widgetInit,drawLabel,selected},
	{4,0,0,{0,360,479,419},"",0,NULL,widgetInit,drawLabel,selected},
	{5,0,0,{0,420,479,479},"",0,NULL,widgetInit,drawLabel,selected},
	{6,0,0,{0,480,479,539},"",0,NULL,widgetInit,drawLabel,selected},
	{7,0,0,{0,540,479,599},"",0,NULL,widgetInit,drawLabel,selected},
	{8,0,0,{0,600,479,659},"",0,NULL,widgetInit,drawLabel,selected},
	{9,0,0,{0,660,479,719},"",0,NULL,widgetInit,drawLabel,selected},
};

static void pageInit(struct UIPage * page)
{
	page->widgetNum = settings.totalparam;
	page->widgetSelected = settings.paramnum;
	page->widgetList[page->widgetSelected].active = 1;
}
	
static void pageReturn(struct UIPage * page)
{
	PageJump(&pSetup);
}

struct UIPage pGeoParam = 
{
	"Geo Spec.",
	1,//char widgetNum;
	-1,
	widgetList,//struct UIWidget * widgetList;
	pageInit,
	pageReturn,
	keyboardEvent,
	touchEvent,
};

