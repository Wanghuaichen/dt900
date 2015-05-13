#ifndef __UI_H
#define __UI_H
#include "stm32f4xx_hal.h"

#define	LIGHTGRAY	0x00f7f7f7
#define DARKGRAY	0x00b2b2b2
#define WHITE			0x00ffffff
#define	BLACK			0x00000000

struct UIInfo
{
	struct PageInfo * PagePtr;
	uint8_t KeyEvent;
	uint8_t TouchEvent;
	uint16_t tpX,tpY;
};

struct Item
{
	uint8_t Type;
	uint8_t Name[20];
	uint32_t Value;
	uint16_t PosX,PosY;
};

struct PageInfo
{
	uint8_t Title[10];
	uint8_t TotalItem;
	uint8_t ItemIndex;
	struct Item *ItemList;	
	void (*Control)();
	void (*Draw)();
};

void drawBtn(struct Item * item);
void UI_Init();
void PageJump(struct PageInfo *page);
void UIControl();
void UIDraw(struct PageInfo *page);
#endif
