#include "ui.h"

static struct UIWidget widgetList[6] =
{
	{1,0,{0,0,0,0},"Main",0,0,NULL,drawTitle},
	{1,0,{100,200,379,259},"Do a test",0,0,NULL,drawButton},
	{1,0,{100,300,379,359},"Test settings",0,0,NULL,drawButton},
	{1,0,{100,400,379,459},"Data files",0,0,NULL,drawButton},
	{1,0,{100,500,379,559},"System preference",0,0,NULL,drawButton},
	{1,0,{100,600,379,659},"About",0,0,NULL,drawButton},
};

struct UIPage pMain = 
{
	NULL,//struct UIPage * parent;
	6,//char widgetNum;
	widgetList,//struct UIWidget * widgetList;
};

