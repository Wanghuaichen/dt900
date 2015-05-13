#include "ui.h"

static struct Item pMainItem[4] = 
{
	{0,"Do a test",0,240,150},
	{0,"Data files",0,240,250},
	{0,"System settings",0,240,350},
	{0,"About",0,240,450},
};

struct PageInfo pMain = 
{
	"Main",		//Title
	4,				//TotalItem
	0,				//ItemIndex
	pMainItem,
	UIControl,
	UIDraw,
};

