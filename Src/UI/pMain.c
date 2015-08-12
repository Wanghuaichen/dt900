#include "ui.h"
#include "cyma568.h"
extern struct UIPage pSettings;
extern struct UIPage pSetup;
extern struct UIPage pSys;

static void key(struct UIWidget* widget)
{
char str[20];	
	switch(widget->widgetIndex)
	{
		case 0:
			PageJump(&pSetup);
			//tpTest();
			//touchreport();
			break;
		case 1:
			PageJump(&pSettings);
			//ping();
			break;
		case 2:
			PageJump(&pSys);
			//HidDes();
			break;
		case 3:
			break;
		default:
			break;
	}
}

static struct UIWidget widgetList[4] =
{
//	{0,1,0,{100,200,379,259},"Do a test",0,NULL,NULL,drawButton,key},
//	{1,1,0,{100,300,379,359},"Geophone settings",0,NULL,NULL,drawButton,key},
//	{2,1,0,{100,400,379,459},"Test configuration",0,NULL,NULL,drawButton,key},
//	{3,1,0,{100,500,379,559},"System preference",0,NULL,NULL,drawButton,key},
//	{4,1,0,{100,600,379,659},"About",0,NULL,NULL,drawButton,key},
	{0,1,0,{100,200,379,259},"Do a test",0,NULL,NULL,drawButton,key},
	{1,1,0,{100,330,379,389},"Geophone settings",0,NULL,NULL,drawButton,key},
	{2,1,0,{100,460,379,519},"System preference",0,NULL,NULL,drawButton,key},
	{3,1,0,{100,590,379,649},"About",0,NULL,NULL,drawButton,key},
};

struct UIPage pMain = 
{
	"Main",
	4,//char widgetNum;
	-1,
	widgetList,//struct UIWidget * widgetList;
	NULL,
	NULL,
	keyboardEvent,
	touchEvent,
};

