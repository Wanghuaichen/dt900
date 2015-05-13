#include "GUI.h"
#include "ui.h"

extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelvetica32;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontVerdana32;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelveticaNeueLT48;

extern struct PageInfo pMain;

struct UIInfo UIInfo;

void UI_Init()
{
	memset(&UIInfo,0,sizeof(UIInfo));
	GUI_SetBkColor(WHITE);
	GUI_Clear();
	PageJump(&pMain);
}

void PageJump(struct PageInfo *page)
{
	UIInfo.PagePtr = page;
	UIDraw(page);
}

void UIDraw(struct PageInfo *page)
{
	uint8_t string[100];
	uint32_t i;
	
//Display Title
	GUI_SetColor(0x00333333);
	GUI_FillRect(0,0,479,149);
	GUI_SetColor(WHITE);
	GUI_SetBkColor(0x00333333);
	GUI_SetFont(&GUI_FontHelveticaNeueLT48);
	sprintf(string,"    %s    ",page->Title);
	GUI_DispStringHCenterAt(string,240,60);
	
//Draw Item
	for(i=0;i<page->TotalItem;i++)
	{
		switch(page->ItemList->Type)
		{
			case 0:
				drawBtn(&page->ItemList[i]);
				break;
			default:
				break;
		}
	}
}

void UIControl()
{
	
}

void drawBtn(struct Item * item)
{
	GUI_SetColor(DARKGRAY);
	GUI_DrawHLine(item->PosY,0,479);
	GUI_DrawHLine(item->PosY+100,0,479);
	GUI_SetColor(WHITE);
	GUI_FillRect(0,item->PosY+1,479,item->PosY+99);
	GUI_SetColor(BLACK);
	GUI_SetBkColor(WHITE);
	GUI_SetFont(&GUI_FontHelvetica32);
	GUI_DispStringHCenterAt(item->Name,240,item->PosY+50-16);
}
	