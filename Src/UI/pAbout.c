#include "ui.h"
#include "DTCC.c"
extern struct UIPage pMain;
extern struct UIInfo UIInfo;

static void pageInit(struct UIPage * page)
{
	char str[20];

	GUI_SetColor(BLACK);
	GUI_SetBkColor(WHITE);
	GUI_Clear();
//	sprintf(str,"%08x  %08x  %08x",uid[0],uid[1],uid[2]);
//	GUI_DispStringAt(str,0,600);
//	sprintf(str,"%02x %02x %02x %02x",t[0],t[1],t[2],t[3]);
//	GUI_DispStringAt(str,0,650);
//	sprintf(str,"%02x %02x %02x %02x",t[4],t[5],t[6],t[7]);
//	GUI_DispStringAt(str,0,700);
//	sprintf(str,"%02x %02x %02x %02x",t[8],t[9],t[10],t[11]);
//	GUI_DispStringAt(str,0,750);
	
	UIFont(1);
	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_VCENTER);
	GUI_DispStringAt("USB Drive Mode",240,76);
	GUI_JPEG_Draw(_acdtcc, sizeof(_acdtcc), 90, 280);
	UIFont(0);
	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_TOP);
	GUI_DispStringAt("www.dynamictech.biz",240,400);
	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_TOP);
	sprintf(str,"PID:%s",UIInfo.uid);
	GUI_DispStringAt(str,240,550);
	
	usbd_OpenMassStorage();
}
	
static void pageReturn(struct UIPage * page)
{
	usbd_CloseMassStorage(); 
	GUI_SetColor(TITLECOLOR);
	GUI_FillRect(0,0,479,119);
	PageJump(&pMain);
}

struct UIPage pAbout = 
{
	"",
	0,//char widgetNum;
	-1,
	NULL,//struct UIWidget * widgetList;
	pageInit,
	pageReturn,
	keyboardEvent,
	touchEvent,
};
