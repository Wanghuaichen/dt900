#include "ui.h"
#include "cyma568.h"
extern struct UIPage pSettings;
extern struct UIPage pSetup;
extern struct UIPage pSys;
extern struct UIPage pAbout;
extern struct UIPage pZVF;

static void key(struct UIWidget* widget)
{
	switch(widget->widgetIndex)
	{
		case 0:
			PageJump(&pSetup);
			//tpTest();
			//touchreport();
			//sysinfo();
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
//			PageJump(&pAbout);
			PageJump(&pZVF);
			break;
		default:
			break;
	}
}

static struct UIWidget widgetList[4] =
{
	{0,1,0,{100,200,379,259},"Do a test",0,NULL,NULL,drawButton,key},
	{1,1,0,{100,330,379,389},"Geophone settings",0,NULL,NULL,drawButton,key},
	{2,1,0,{100,460,379,519},"System preference",0,NULL,NULL,drawButton,key},
	{3,1,0,{100,590,379,649},"USB storage",0,NULL,NULL,drawButton,key},
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

