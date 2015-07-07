#ifndef __UI_H
#define __UI_H
#include "stm32f4xx_hal.h"
#include "GUI.h"
#define	LIGHTGRAY	0x00f7f7f7
#define DARKGRAY	0x00b2b2b2
#define WHITE			0x00ffffff
#define	BLACK			0x00000000
#define TITLECOLOR 0x002d2d2d

struct UIInfo
{
	struct UIPage * PagePtr;
	uint8_t KeyEvent;
	uint8_t TouchEvent;
	uint16_t tpX,tpY;
};

struct UIWidget
{
	char enable;
	char active;
	GUI_RECT rect;//x0,y0,x1,y1
	char widgetTitle[20];
	float widgetValue;
	char widgetFMT;
	void (*widgetInit)(struct UIWidget* widget);
	void (*widgetDraw)(struct UIWidget* widget);
};

struct UIPage
{
	struct UIPage * parent;
	char widgetNum;
	struct UIWidget * widgetList;
	void (*pageInit)(struct UIPage* page);
};

void UI_Init();
void PageJump(struct UIPage *page);
void UIDraw(struct UIPage *page);
void drawTitle(struct UIWidget* widget);
void drawButton(struct UIWidget* widget);
void drawLabel(struct UIWidget* widget);
#endif
