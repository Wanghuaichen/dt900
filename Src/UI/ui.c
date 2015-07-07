#include "GUI.h"
#include "ui.h"

extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelvetica32;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontVerdana32;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelveticaNeueLT48;

extern struct UIPage pMain;
extern struct UIPage pSettings;

struct UIInfo UIInfo;


void UI_Init()
{
	memset(&UIInfo,0,sizeof(UIInfo));
	GUI_SetBkColor(WHITE);
	GUI_Clear();
	GUI_SetColor(TITLECOLOR);
	GUI_FillRect(0,0,479,119);
	PageJump(&pMain);
}

void PageJump(struct UIPage *page)
{
	UIInfo.PagePtr = page;
	GUI_SetColor(TITLECOLOR);
	GUI_FillRect(0,60,479,119);
	GUI_SetColor(WHITE);
	GUI_FillRect(0,120,479,799);
	if(page->pageInit)
		(page->pageInit)(page);
	UIDraw(page);
}

void UIDraw(struct UIPage *page)
{
	uint32_t i;
	
	for(i=0;i<page->widgetNum;i++)
	{
		if(page->widgetList[i].widgetInit)
			page->widgetList[i].widgetInit(&page->widgetList[i]);
		
		if(page->widgetList[i].widgetDraw)
			page->widgetList[i].widgetDraw(&page->widgetList[i]);
	}
}


void drawTitle(struct UIWidget* widget)
{
	GUI_SetColor(WHITE);
	GUI_SetBkColor(TITLECOLOR);
	GUI_SetFont(&GUI_FontHelveticaNeueLT48);
	GUI_DispStringHCenterAt(widget->widgetTitle,240,60);
}

void drawButton(struct UIWidget* widget)
{
	GUI_SetColor(0x00606060);
	GUI_SetPenSize(2);
	GUI_AA_DrawRoundedRect(widget->rect.x0,widget->rect.y0,widget->rect.x1,widget->rect.y1,(widget->rect.y1-widget->rect.y0)/2);
	
	GUI_SetColor(BLACK);
	GUI_SetBkColor(WHITE);
	GUI_SetFont(&GUI_FontHelvetica32);	
	GUI_DispStringHCenterAt(widget->widgetTitle,240,(widget->rect.y1+widget->rect.y0)/2-16);
}

void drawLabel(struct UIWidget* widget)
{
	if(!widget->enable)
		return;
	GUI_SetColor(BLACK);
	GUI_SetBkColor(WHITE);
	GUI_SetFont(&GUI_FontHelvetica32);	
	GUI_DispStringHCenterAt(widget->widgetTitle,240,(widget->rect.y1+widget->rect.y0)/2-16);
}
	