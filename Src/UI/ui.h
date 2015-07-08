#ifndef __UI_H
#define __UI_H
#include "stm32f4xx_hal.h"
#include "GUI.h"
#define	LIGHTGRAY	0x00f7f7f7
#define DARKGRAY	0x00b2b2b2
#define WHITE			0x00ffffff
#define	BLACK			0x00000000
#define TITLECOLOR 0x002d2d2d

#define KEY_NONE 0x0
#define KEY_CANCEL 0x1
#define KEY_DOWN 0x2
#define KEY_OK 0x4


struct UIInfo
{
	struct UIPage * PagePtr;
	uint8_t KeyEvent;
	char currentKey;
	char lastKey;
	uint32_t keyCombo;
	
	uint8_t TouchEvent;
	uint16_t tpX,tpY;
};

struct UIWidget
{
	char widgetIndex;
	char enable;
	char active;
	GUI_RECT rect;//x0,y0,x1,y1
	char widgetTitle[20];
	float widgetValue;
	void* widgetPtr;
	void (*widgetInit)(struct UIWidget* widget);
	void (*widgetDraw)(struct UIWidget* widget);
	void (*widgetAct)(struct UIWidget* widget);
};

struct UIPage
{
	char pageTitle[20];
	struct UIPage * parent;
	char widgetNum;
	short widgetSelected;
	struct UIWidget * widgetList;
	void (*pageInit)(struct UIPage* page);
	void (*keyboard)(struct UIPage* page);
	void (*touch)(struct UIPage* page);
};

void UI_Init();

void UIKeyboard();
void UITouch();
void UIEventManager();

void keyboardEvent(struct UIPage *page);
void touchEvent(struct UIPage *page);

void PageJump(struct UIPage *page);
void UIDraw(struct UIPage *page);
void drawButton(struct UIWidget* widget);
void drawLabel(struct UIWidget* widget);
void drawSLabel(struct UIWidget* widget);
#endif
