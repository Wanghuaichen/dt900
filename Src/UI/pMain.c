#include "ui.h"

static struct Item item[4] = 
{
	{0,"name1",0,240,200},
	{0,"name2",0,240,300},
	{0,"name3",0,240,400},
	{0,"name4",0,240,500},
};

struct PageInfo pMain = 
{
	"Main",		//Title
	4,				//TotalItem
	0,				//ItemIndex
	item,
	UIControl,
	UIDraw,
};

