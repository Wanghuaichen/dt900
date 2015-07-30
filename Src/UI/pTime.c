#include "ui.h"
#include "geotest.h"
#include <stdlib.h>
#include "rtc.h"

extern struct UIPage pKeyboard;
extern struct KBInfo kbInfo;
extern struct GeoParam geoparam[10];
extern struct Settings settings;
extern struct UIInfo UIInfo;
extern struct UIPage pSys;
static char StringArray[6][20];
struct UIPage pTime;
static struct UIWidget widgetList[7];

RTC_DateTypeDef Date;
RTC_TimeTypeDef Time;
static int timeflag = 0;

static void kbCallBack()
{
	char str[20];
	unsigned int val = atoi(kbInfo.kbBuff);
	switch(pTime.widgetSelected)
	{
		case 0:
			val = val>9999 ? 9999 : val<1 ? 1 : val;
			Date.Year = val;
			break;
		case 1:
			val = val>12 ? 12 : val<1 ? 1 : val;
			Date.Month = val;
			break;
		case 2:
			val = val>31 ? 31 : val<1 ? 1 : val;
			Date.Date = val;
			break;
		case 3:
			val = val>23 ? 23 : val;
			Time.Hours = val;
		case 4:
			val = val>59 ? 59 : val;
			Time.Minutes = val;
		case 5:
			val = val>59 ? 59 : val;
			Time.Seconds = val;
			break;
		default:
			break;
	}
	UIInfo.flagSettings = 1;
}

static void goSubSettings(struct UIWidget * widget)
{
	kbInfo.kbParent = &pTime;
	strcpy(kbInfo.kbTitle,widget->widgetTitle);
	strcpy(kbInfo.kbBuff,widget->widgetPtr);
	kbInfo.kbCallBack = kbCallBack;
	PageJump(&pKeyboard);
}

static void widgetInit(struct UIWidget * widget)
{
	switch(widget->widgetIndex)
	{
		case 0:
			sprintf(widget->widgetPtr,"%d",Date.Year);
			break;
		case 1:
			sprintf(widget->widgetPtr,"%d",Date.Month);
			break;
		case 2:
			sprintf(widget->widgetPtr,"%d",Date.Date);
			break;
		case 3:
			sprintf(widget->widgetPtr,"%d",Time.Hours);
			break;
		case 4:
			sprintf(widget->widgetPtr,"%d",Time.Minutes);
			break;
		case 5:
			sprintf(widget->widgetPtr,"%d",Time.Seconds);
			break;
		default:
			break;
	}
}

static void pageReturn(struct UIPage * page)
{
	timeflag = 0;
	PageJump(&pSys);
}

static void settime()
{
	RTC_SetDateTime(&Date,&Time);
	timeflag = 0;
	PageJump(&pSys);
}
	
static struct UIWidget widgetList[7] =
{
	{0,1,0,{0,120,479,179},"Year(yy)",0,StringArray[0],widgetInit,drawSLabel,goSubSettings},
	{1,1,0,{0,180,479,239},"Month(mm)",0,StringArray[1],widgetInit,drawSLabel,goSubSettings},
	{2,1,0,{0,240,479,299},"Date(dd)",0,StringArray[2],widgetInit,drawSLabel,goSubSettings},
	{3,1,0,{0,300,479,359},"Hours(hh)",0,StringArray[3],widgetInit,drawSLabel,goSubSettings},
	{4,1,0,{0,360,479,419},"Minutes(mm)",0,StringArray[4],widgetInit,drawSLabel,goSubSettings},
	{5,1,0,{0,420,479,479},"Seconds(ss)",0,StringArray[5],widgetInit,drawSLabel,goSubSettings},
	{6,1,0,{100,540,379,600},"Set Time",0,NULL,widgetInit,drawButton,settime},
};

static void pageInit(struct UIPage * page)
{
	if(!timeflag)
	{
		memcpy(&Date,&rtcDate,sizeof(rtcDate));
		memcpy(&Time,&rtcTime,sizeof(rtcTime));
		timeflag = 1;
	}
}

struct UIPage pTime = 
{
	"Date & Time",
	7,//char widgetNum;
	-1,
	widgetList,//struct UIWidget * widgetList;
	pageInit,
	pageReturn,
	keyboardEvent,
	touchEvent,
};

