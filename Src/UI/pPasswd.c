#include "ui.h"
#include <math.h>
#include "geotest.h"
#include "flash.h"

static char StringArray[2][20];
static struct UIWidget widgetList[2];
struct UIPage pPasswd;

extern struct UIPage pMain;
extern struct UIInfo UIInfo;
extern struct UIPage pKeyboard;
extern struct KBInfo kbInfo;
extern struct Settings settings;


static void kbCallBack()
{
	settings.passwd = atol(kbInfo.kbBuff);
}

static void goSubSettings(struct UIWidget * widget)
{
	kbInfo.kbParent = &pPasswd;
	strcpy(kbInfo.kbTitle,widget->widgetTitle);
	strcpy(kbInfo.kbBuff,widget->widgetPtr);
	kbInfo.kbCallBack = kbCallBack;
	kbInfo.strlength = 16;
	PageJump(&pKeyboard);
}


static void widgetInit(struct UIWidget * widget)
{
	switch(widget->widgetIndex)
	{
		case 0:
			strcpy(widget->widgetPtr,UIInfo.uid);
			break;
		case 1:
			sprintf(widget->widgetPtr,"%ld",settings.passwd);
			break;
		default:
			break;
	}
}

static void pageReturn(struct UIPage * page)
{
	PageJump(&pMain);
	FlashPasswd(settings.passwd);
}

static struct UIWidget widgetList[2] =
{
	{0,1,0,{0,120,479,179}," ",0,StringArray[0],widgetInit,drawSLabel,NULL},
	{1,1,0,{0,180,479,239}," ",0,StringArray[1],widgetInit,drawSLabel,goSubSettings},
};

struct UIPage pPasswd = 
{
	"",
	2,//char widgetNum;
	-1,
	widgetList,//struct UIWidget * widgetList;
	NULL,
	pageReturn,
	keyboardEvent,
	touchEvent,
};

