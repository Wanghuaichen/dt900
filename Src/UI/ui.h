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

#define TOUCH_NONE 0x0
#define TOUCH_TOUCHDOWN 0x1
#define TOUCH_DISPLACEMENT 0x2
#define TOUCH_LIFTOFF 0x3

struct KBInfo
{
struct UIPage *kbParent;
char kbTitle[20];
char kbBuff[40];
void (*kbCallBack)();
char strlength;
};

struct UIInfo
{
	uint32_t chargeflag;
	uint8_t insleep;
	
	struct UIPage * PagePtr;
	
	uint8_t KeyEvent;
	uint8_t currentKey;
	uint8_t lastKey;
	uint32_t keyCombo;
	
	uint8_t TouchEvent;
	uint8_t widgetactive;
	uint32_t touchCombo;
	uint16_t tpX,tpY;
	
	char uid[10];
};

struct UIWidget
{
	char widgetIndex;
	char enable;
	char active;
	GUI_RECT rect;//x0,y0,x1,y1
	char widgetTitle[20];
	int widgetValue;
	void* widgetPtr;
	void (*widgetInit)(struct UIWidget* widget);
	void (*widgetDraw)(struct UIWidget* widget);
	void (*widgetAct)(struct UIWidget* widget);
};

struct UIPage
{
	char pageTitle[20];
	char widgetNum;
	short widgetSelected;
	struct UIWidget * widgetList;
	void (*pageInit)(struct UIPage* page);
	void (*pageReturn)(struct UIPage* page);
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
void drawBox(struct UIWidget* widget);
struct UIInfo * getUIInfo();
void UIFont(int mode);
void getuid();
void UITouchClear();
#endif
