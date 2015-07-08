#include "ui.h"

extern struct UIPage pSettings;

static void key1(struct UIWidget* widget){}
static void key2(struct UIWidget* widget){PageJump(&pSettings);}
static void key3(struct UIWidget* widget){}
static void key4(struct UIWidget* widget){}
static void key5(struct UIWidget* widget){}
static struct UIWidget widgetList[5] =
{
	{0,1,0,{100,200,379,259},"Do a test",0,NULL,NULL,drawButton,key1},
	{1,1,0,{100,300,379,359},"Geophone settings",0,NULL,NULL,drawButton,key2},
	{2,1,0,{100,400,379,459},"Data files",0,NULL,NULL,drawButton,key3},
	{3,1,0,{100,500,379,559},"System preference",0,NULL,NULL,drawButton,key4},
	{4,1,0,{100,600,379,659},"About",0,NULL,NULL,drawButton,key5},
};

struct UIPage pMain = 
{
	"Main",
	NULL,//struct UIPage * parent;
	5,//char widgetNum;
	-1,
	widgetList,//struct UIWidget * widgetList;
	NULL,
	keyboardEvent,
	touchEvent,
};

