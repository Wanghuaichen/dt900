#include "ui.h"
#include "geotest.h"

extern struct UIPage pMain;
extern struct GeoParam geoparam[10];
extern struct Settings settings;

static struct UIWidget widgetList[11] =
{
	{1,0,{0,0,0,0},"Geophone Settings",0,0,NULL,drawTitle},
	{0,0,{0,120,0,179},"",0,0,NULL,drawLabel},
	{0,0,{0,180,0,239},"",0,0,NULL,drawLabel},
	{0,0,{0,240,0,299},"",0,0,NULL,drawLabel},
	{0,0,{0,300,0,359},"",0,0,NULL,drawLabel},
	{0,0,{0,360,0,419},"",0,0,NULL,drawLabel},
	{0,0,{0,420,0,479},"",0,0,NULL,drawLabel},
	{0,0,{0,480,0,539},"",0,0,NULL,drawLabel},
	{0,0,{0,540,0,599},"",0,0,NULL,drawLabel},
	{0,0,{0,600,0,659},"",0,0,NULL,drawLabel},
	{0,0,{0,660,0,719},"",0,0,NULL,drawLabel},
};

static void pageInit(struct UIPage * page)
{
	int i;
	
	for(i=0;i<10;i++)
	{
		if(i<settings.totalparam)
		{
			page->widgetList[i+1].enable=1;
			sprintf(page->widgetList[i+1].widgetTitle,geoparam[i].type);
		}
		else if(i==settings.totalparam)
		{
			page->widgetList[i+1].enable=1;
			sprintf(page->widgetList[i+1].widgetTitle,"<NEW>");
		}
		else
		{
			page->widgetList[i+1].enable=0;
		}
	}
}
	
struct UIPage pSettings = 
{
	&pMain,//struct UIPage * parent;
	11,//char widgetNum;
	widgetList,//struct UIWidget * widgetList;
	pageInit,
};

