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
			break;
		case 1:
			PageJump(&pZVF);
			break;
		case 2:
			PageJump(&pSettings);
			break;
		case 3:
			PageJump(&pSys);
			break;
		case 4:
			PageJump(&pAbout);
			break;
		default:
			break;
	}
}

static struct UIWidget widgetList[5] =
{
	{0,1,0,{100,200,379,259},"Geophone Test",0,NULL,NULL,drawButton,key},
	{1,1,0,{100,300,379,359},"Impedance Plot",0,NULL,NULL,drawButton,key},
	{2,1,0,{100,400,379,459},"Geophone Settings",0,NULL,NULL,drawButton,key},
	{3,1,0,{100,500,379,559},"System Preference",0,NULL,NULL,drawButton,key},
	{4,1,0,{100,600,379,659},"USB Drive",0,NULL,NULL,drawButton,key},
//	{0,1,0,{100,200,379,259},"Do a test",0,NULL,NULL,drawButton,key},
//	{1,1,0,{100,330,379,389},"Geophone settings",0,NULL,NULL,drawButton,key},
//	{2,1,0,{100,460,379,519},"System preference",0,NULL,NULL,drawButton,key},
//	{3,1,0,{100,590,379,649},"USB storage",0,NULL,NULL,drawButton,key},
};

struct UIPage pMain = 
{
	"Main",
	5,//char widgetNum;
	-1,
	widgetList,//struct UIWidget * widgetList;
	NULL,
	NULL,
	keyboardEvent,
	touchEvent,
};

