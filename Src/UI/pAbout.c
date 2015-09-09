#include "ui.h"
#include "DTCC.c"
extern struct UIPage pMain;

static void pageInit(struct UIPage * page)
{
	int i,temp;
	char str[20];
	char id[20];

	uint8_t * t = (uint8_t*)0x1FFF7A10;
	GUI_SetColor(BLACK);
	GUI_SetBkColor(WHITE);
	
//	sprintf(str,"%08x  %08x  %08x",uid[0],uid[1],uid[2]);
//	GUI_DispStringAt(str,0,600);
//	sprintf(str,"%02x %02x %02x %02x",t[0],t[1],t[2],t[3]);
//	GUI_DispStringAt(str,0,650);
//	sprintf(str,"%02x %02x %02x %02x",t[4],t[5],t[6],t[7]);
//	GUI_DispStringAt(str,0,700);
//	sprintf(str,"%02x %02x %02x %02x",t[8],t[9],t[10],t[11]);
//	GUI_DispStringAt(str,0,750);
	
	for(i=0;i<4;i++)
	{
		temp = (int)(t[i]>>4)+(t[i+4]>>4)+(t[i+8]>>4);
		temp %= 36;
		id[i*2+2] = temp>9 ? temp+55: temp+0x30;
		temp = (int)(t[i]&0xf)+(t[i+4]&0xf)+(t[i+8]&0xf);
		temp %= 36;
		id[i*2+3] = temp>9 ? temp+55: temp+0x30;
	}
	id[0] = 'A';
	id[1] = '1';
	id[10] = '\0';
	GUI_JPEG_Draw(_acdtcc, sizeof(_acdtcc), 90, 280);
	sprintf(str,"PID:%s",id);
	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_TOP);
	GUI_DispStringAt("www.dynamictech.biz",240,400);
	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_TOP);
	GUI_DispStringAt(str,240,550);
	
	usbd_OpenMassStorage();
}
	
static void pageReturn(struct UIPage * page)
{
	usbd_CloseMassStorage(); 
	PageJump(&pMain);
}

struct UIPage pAbout = 
{
	"USB Storage Mode",
	0,//char widgetNum;
	-1,
	NULL,//struct UIWidget * widgetList;
	pageInit,
	pageReturn,
	keyboardEvent,
	touchEvent,
};
