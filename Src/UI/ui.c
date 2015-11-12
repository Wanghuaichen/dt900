#include "GUI.h"
#include "ui.h"
#include "pwr.h"
#include "lcd.h"
#include "geotest.h"
#include "cyma568.h"
#include "flash.h"

//#define BITMAP
//#define NONTOUCHCLEAR

extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelvetica32;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelveticaNeueLT48;

extern struct UIPage pMain;
extern struct UIPage pAbout;
extern struct UIPage pKeyboard;
extern struct UIPage pZVF;

extern struct Settings settings;

struct UIInfo UIInfo;

void getuid()
{
	uint8_t * t = (uint8_t*)0x1FFF7A10-0x41;
	int i,temp;
	
	for(i=0;i<4;i++)
	{
		temp = (int)(t[0x41+i]>>4)+(t[0x41+i+4]>>4)+(t[0x41+i+8]>>4);
		temp %= 36;
		UIInfo.uid[i*2] = temp>9 ? temp+55: temp+0x30;
		temp = (int)(t[0x41+i]&0xf)+(t[0x41+i+4]&0xf)+(t[0x41+i+8]&0xf);
		temp %= 36;
		UIInfo.uid[i*2+1] = temp>9 ? temp+55: temp+0x30;
	}
	UIInfo.uid[8] = '\0';
}

struct UIInfo * getUIInfo()
{
	return &UIInfo;
}

void UIKeyboard()
{
	UIInfo.currentKey = GPIOA->IDR&0x7;
	
	if(UIInfo.currentKey!=UIInfo.lastKey)
	{
		UIInfo.keyCombo=0;
		UIInfo.KeyEvent = UIInfo.currentKey;
	}
	else
	{
		UIInfo.keyCombo++;
		if(UIInfo.keyCombo%500==0)
		{
				UIInfo.KeyEvent = UIInfo.currentKey;
		}
	}
	UIInfo.lastKey = UIInfo.currentKey;
}

void UITouchClear()
{
#ifndef NONTOUCHCLEAR
	while(tpScan());
//		HAL_Delay(1);
#endif
	UIInfo.TouchEvent = TOUCH_NONE;
}

//void UITouch()
//{
//	tpScan();
//	
//}
	
void UIEventManager()
{
	UIKeyboard();
	if(UIInfo.KeyEvent==KEY_NONE)
		tpScan();
	
	if(UIInfo.keyCombo>60000*settings.sleeptime && !UIInfo.insleep)
	{
		LCD_PWR(0);
		UIInfo.insleep=1;
	}
	
	if(UIInfo.keyCombo>60000*settings.shuttime || (UIInfo.keyCombo==2500 && UIInfo.KeyEvent==KEY_CANCEL))
	{
		LCD_PWR(0);
		FlashProgram();
		DP_EN(0);
	}
	
	if(UIInfo.insleep)
	{
		if(UIInfo.TouchEvent==TOUCH_LIFTOFF || (UIInfo.KeyEvent==KEY_NONE && UIInfo.keyCombo==0))
		{
			UIInfo.TouchEvent=UIInfo.KeyEvent=0;
			UIInfo.insleep = 0;
			UIInfo.keyCombo = 0;
			LCD_PWR(1);
		}
		else
			return;
	}
	
	if(UIInfo.TouchEvent!=TOUCH_NONE)
		UIInfo.PagePtr->touch(UIInfo.PagePtr);
	if(UIInfo.KeyEvent!=KEY_NONE)
		UIInfo.PagePtr->keyboard(UIInfo.PagePtr);
}

void touchEvent(struct UIPage *page)
{
	int index;
	
	if(UIInfo.TouchEvent==TOUCH_TOUCHDOWN)
	{
		UIInfo.keyCombo = 0;
		if(page->widgetSelected>=0)
		{
			page->widgetList[page->widgetSelected].active=0;
			if(page->widgetList[page->widgetSelected].widgetDraw)
				page->widgetList[page->widgetSelected].widgetDraw(&page->widgetList[page->widgetSelected]);
			page->widgetSelected = -1;
		}
#ifdef BITMAP
if(UIInfo.tpY<120 && UIInfo.tpX>400)
	SaveBitmap();
#endif

		if(UIInfo.tpY<120 && UIInfo.tpX<120)
		{
			if(page->pageReturn)
			{
				page->pageReturn(page);
			}
			return;
		}
		
		for(index=0;index<page->widgetNum;index++)
		{
			if(UIInfo.tpX>=page->widgetList[index].rect.x0 && UIInfo.tpX<=page->widgetList[index].rect.x1 
				&& UIInfo.tpY>=page->widgetList[index].rect.y0 && UIInfo.tpY<=page->widgetList[index].rect.y1)
			{
				if(page->widgetList[index].enable)
				{
					UIInfo.widgetactive = 1;
					page->widgetSelected = index;
					page->widgetList[page->widgetSelected].active=1;
					if(page->widgetList[page->widgetSelected].widgetDraw)
						page->widgetList[page->widgetSelected].widgetDraw(&page->widgetList[page->widgetSelected]);
				}
				break;
			}
		}
		if(page->widgetSelected>=0 && page==&pKeyboard)
		{
			if(page->widgetList[page->widgetSelected].widgetAct)
				page->widgetList[page->widgetSelected].widgetAct(&page->widgetList[page->widgetSelected]);
		}
	}
	else if(UIInfo.TouchEvent==TOUCH_DISPLACEMENT && UIInfo.widgetactive)
	{
		UIInfo.keyCombo++;
		if(page==&pKeyboard || (page==&pZVF && page->widgetSelected==0))
		{
			if(page->widgetList[page->widgetSelected].widgetAct)
				page->widgetList[page->widgetSelected].widgetAct(&page->widgetList[page->widgetSelected]);
		}
		else if(UIInfo.keyCombo>1500)
		{
			page->widgetList[page->widgetSelected].active=0;
			if(page->widgetList[page->widgetSelected].widgetDraw)
				page->widgetList[page->widgetSelected].widgetDraw(&page->widgetList[page->widgetSelected]);
			page->widgetSelected = -1;
			UIInfo.widgetactive = 0;
		}
	}
	else if(UIInfo.TouchEvent==TOUCH_LIFTOFF)
	{
		UIInfo.keyCombo=0;
		UIInfo.TouchEvent=TOUCH_NONE;
		if(UIInfo.widgetactive)
		{
			UIInfo.widgetactive = 0;
			if(page==&pKeyboard)
			{
				page->widgetList[page->widgetSelected].active=0;
				if(page->widgetList[page->widgetSelected].widgetDraw)
					page->widgetList[page->widgetSelected].widgetDraw(&page->widgetList[page->widgetSelected]);
				page->widgetSelected = -1;
			}
			else if(page->widgetList[page->widgetSelected].widgetAct)
					page->widgetList[page->widgetSelected].widgetAct(&page->widgetList[page->widgetSelected]);
		}
	}
}
	
	
void keyboardEvent(struct UIPage *page)
{
	if(UIInfo.KeyEvent==KEY_CANCEL && UIInfo.keyCombo==0)
	{
		if(page->widgetSelected>=0)
		{
			page->widgetList[page->widgetSelected].active=0;
			if(page->widgetList[page->widgetSelected].widgetDraw)
				page->widgetList[page->widgetSelected].widgetDraw(&page->widgetList[page->widgetSelected]);
			page->widgetSelected = -1;
		}
		if(page->pageReturn)
		{
			page->pageReturn(page);
		}
	}
	else if(UIInfo.KeyEvent==KEY_DOWN && page->widgetNum>0)
	{
		if(page->widgetSelected>=0 && page->widgetNum>1)
		{
			page->widgetList[page->widgetSelected].active=0;
			if(page->widgetList[page->widgetSelected].widgetDraw)
				page->widgetList[page->widgetSelected].widgetDraw(&page->widgetList[page->widgetSelected]);
		}
		do
		page->widgetSelected = page->widgetSelected == page->widgetNum-1 ? 0 : page->widgetSelected+1;
		while(!page->widgetList[page->widgetSelected].enable || page->widgetList[page->widgetSelected].active);
		page->widgetList[page->widgetSelected].active=1;
		if(page->widgetList[page->widgetSelected].widgetDraw)
			page->widgetList[page->widgetSelected].widgetDraw(&page->widgetList[page->widgetSelected]);
	}
	else if(UIInfo.KeyEvent==KEY_OK && UIInfo.keyCombo==0)
	{
		if(page->widgetSelected>=0 && page->widgetList[page->widgetSelected].widgetAct)
			page->widgetList[page->widgetSelected].widgetAct(&page->widgetList[page->widgetSelected]);
	}	
	else if(UIInfo.KeyEvent==KEY_OK && page==&pKeyboard)
	{
		if(page->widgetSelected>=0 && page->widgetList[page->widgetSelected].widgetAct)
			page->widgetList[page->widgetSelected].widgetAct(&page->widgetList[page->widgetSelected]);
	}
	UIInfo.KeyEvent = KEY_NONE;
}

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

void UIFont(int mode)
{
	if(mode)
		GUI_SetFont(&GUI_FontHelveticaNeueLT48);
	else
		GUI_SetFont(&GUI_FontHelvetica32);
		
}

void UIDraw(struct UIPage *page)
{
	uint32_t i;
	const GUI_POINT aPoints[] ={{ 0, 10},{ 10, 20},{ 10, 0}};
	
	GUI_SetColor(WHITE);
	GUI_SetBkColor(TITLECOLOR);
	GUI_SetFont(&GUI_FontHelveticaNeueLT48);
	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_VCENTER);
	GUI_DispStringAt(page->pageTitle,240,76);
	
	GUI_SetColor((page==&pMain || page==&pAbout) ? TITLECOLOR : WHITE);
	GUI_FillPolygon(aPoints, GUI_COUNTOF(aPoints), 30, 70);
	
	for(i=0;i<page->widgetNum;i++)
	{
		if(page->widgetList[i].widgetInit)
			page->widgetList[i].widgetInit(&page->widgetList[i]);
		
		if(page->widgetList[i].widgetDraw)
			page->widgetList[i].widgetDraw(&page->widgetList[i]);
	}
}

void drawButton(struct UIWidget* widget)
{
	GUI_SetColor(WHITE);
	GUI_FillRect(widget->rect.x0,widget->rect.y0,widget->rect.x1,widget->rect.y1);	
	if(widget->enable)
	{
		GUI_SetPenSize(2);
		GUI_SetColor(widget->active ? 0x003fceff : 0x002f2f2f);
		GUI_AA_DrawRoundedRect(widget->rect.x0+1,widget->rect.y0+1,widget->rect.x1-1,widget->rect.y1-1,(widget->rect.y1-widget->rect.y0-2)/2);
		GUI_SetColor(widget->active ? 0x002fbeff : BLACK);
		GUI_SetBkColor(WHITE);
		GUI_SetFont(&GUI_FontHelvetica32);	
		GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_VCENTER);
		GUI_DispStringAt(widget->widgetTitle,(widget->rect.x1+widget->rect.x0)/2,(widget->rect.y1+widget->rect.y0)/2);
	}
}


void drawLabel(struct UIWidget* widget)
{
	if(!widget->enable)
			return;
	GUI_SetColor(WHITE);
	GUI_FillRect(widget->rect.x0,widget->rect.y0,widget->rect.x1,widget->rect.y1);
	GUI_SetColor(widget->active ? 0x002fbeff : BLACK);
	GUI_SetBkColor(WHITE);
	GUI_SetFont(&GUI_FontHelvetica32);	
	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_VCENTER);
	GUI_DispStringAt(widget->widgetTitle,(widget->rect.x1+widget->rect.x0)/2,(widget->rect.y1+widget->rect.y0)/2);
	GUI_SetColor(DARKGRAY);
	GUI_DrawHLine(widget->rect.y1,20,459);
}

void drawSLabel(struct UIWidget* widget)
{
	GUI_SetColor(WHITE);
	GUI_FillRect(widget->rect.x0,widget->rect.y0,widget->rect.x1,widget->rect.y1);
	
	GUI_SetFont(&GUI_FontHelvetica32);
	
	GUI_SetColor(widget->enable ? (widget->active ? 0x002fbeff : BLACK) : 0x00b0b0b0);
	GUI_SetBkColor(WHITE);
	GUI_SetTextAlign(GUI_TA_RIGHT | GUI_TA_VCENTER);	
	GUI_DispStringAt((char *)widget->widgetPtr,439,(widget->rect.y1+widget->rect.y0)/2);
	
	GUI_SetColor(widget->enable ? (widget->active ? 0x002fbeff : BLACK) : 0x00b0b0b0);
	GUI_SetBkColor(WHITE);
	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);	
	GUI_DispStringAt(widget->widgetTitle,40,(widget->rect.y1+widget->rect.y0)/2);
	
	GUI_SetColor(DARKGRAY);
	GUI_DrawHLine(widget->rect.y1,widget->rect.x0+20,widget->rect.x1-20);
}

void drawBox(struct UIWidget * widget)
{
	char str[40];
	GUI_SetColor(WHITE);
	GUI_FillRect(widget->rect.x0,widget->rect.y0,widget->rect.x1,widget->rect.y1);	
	GUI_SetPenSize(2);
	GUI_SetColor(widget->active ? 0x003fceff : 0x002f2f2f);
//	GUI_AA_DrawRoundedRect(widget->rect.x0+1,widget->rect.y0+1,widget->rect.x1-1,widget->rect.y1-1,
//	(widget->rect.y1-widget->rect.y0-2)/4);
	//GUI_DrawHLine(widget->rect.y1,widget->rect.x0,widget->rect.x1);
	
	GUI_SetColor(widget->active ? 0x002fbeff : BLACK);
	GUI_SetBkColor(WHITE);
	
	sprintf(str,"%s:%s",widget->widgetTitle,widget->widgetPtr);
	GUI_SetFont(&GUI_FontHelvetica32);	
	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
	GUI_DispStringAt(str,widget->rect.x0,(widget->rect.y1+widget->rect.y0)/2);
//	
//	GUI_DispStringAt(widget->widgetTitle,widget->rect.x0,(widget->rect.y1+widget->rect.y0)/2);
//	GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
//	GUI_DispStringAt(widget->widgetPtr,widget->rect.x0+100,(widget->rect.y1+widget->rect.y0)/2);
}
	