#include "ui.h"
#include "geotest.h"
#include "flash.h"

extern struct UIPage pMain;
extern struct UIPage pSubSettings;
extern struct GeoParam geoparam[10];
extern struct Settings settings;
extern struct UIInfo UIInfo;

static void goSubSettings(struct UIWidget * widget)
{
	if(!widget->enable)
		return;
	PageJump(&pSubSettings);
}

static void widgetInit(struct UIWidget * widget)
{
	if(widget->widgetIndex < settings.totalparam)
	{
		strcpy(widget->widgetTitle,geoparam[widget->widgetIndex].type);
		widget->enable = 1;
	}
	else if(widget->widgetIndex == settings.totalparam)
	{
		sprintf(widget->widgetTitle,"<NEW>");
		widget->enable = 1;
	}
	else
		widget->enable = 0;
}

static struct UIWidget widgetList[10] =
{
	{0,0,0,{0,120,479,179},"",0,NULL,widgetInit,drawLabel,goSubSettings},
	{1,0,0,{0,180,479,239},"",0,NULL,widgetInit,drawLabel,goSubSettings},
	{2,0,0,{0,240,479,299},"",0,NULL,widgetInit,drawLabel,goSubSettings},
	{3,0,0,{0,300,479,359},"",0,NULL,widgetInit,drawLabel,goSubSettings},
	{4,0,0,{0,360,479,419},"",0,NULL,widgetInit,drawLabel,goSubSettings},
	{5,0,0,{0,420,479,479},"",0,NULL,widgetInit,drawLabel,goSubSettings},
	{6,0,0,{0,480,479,539},"",0,NULL,widgetInit,drawLabel,goSubSettings},
	{7,0,0,{0,540,479,599},"",0,NULL,widgetInit,drawLabel,goSubSettings},
	{8,0,0,{0,600,479,659},"",0,NULL,widgetInit,drawLabel,goSubSettings},
	{9,0,0,{0,660,479,719},"",0,NULL,widgetInit,drawLabel,goSubSettings},
};

static void pageInit(struct UIPage * page)
{
	page->widgetNum = settings.totalparam<10 ? settings.totalparam+1 : 10;
}
	
static void pageReturn(struct UIPage * page)
{
	PageJump(&pMain);
	UIInfo.flagSettings = 1;
}

struct UIPage pSettings = 
{
	"Geophone Settings",
	1,//char widgetNum;
	-1,
	widgetList,//struct UIWidget * widgetList;
	pageInit,
	pageReturn,
	keyboardEvent,
	touchEvent,
};

