#include "GUI.h"
#include "ui.h"
#include "pMain.c"

struct UIInfo UIInfo;

void UI_Init()
{
	PageJump(&pMain);
}

void PageJump(struct PageInfo *page)
{
	UIInfo.PagePtr = page;
	page->Draw(page);
}

void UIDraw(struct PageInfo *page)
{
	
}

void UIControl()
{
	
}

	