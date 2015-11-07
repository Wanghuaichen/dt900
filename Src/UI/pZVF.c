#include "ui.h"
#include <math.h>
#include "geotest.h"
#define NUM 32
extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelvetica32;
extern struct UIPage pMain;

static struct UIWidget widgetList[2];

static int color = 0x00f7823e;
static int oriX=220;
static int oriY=40;
static int width = 500;
static int height = 400;
static int yinterval = 100;
static int xcoordinate[23] = {1,2,3,4,5,6,7,8,9,10,20,30,40,50,60,70,80,90,100,200,300,400,500};
static int xvalue[NUM] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,30,40,50,60,70,80,90,100,200,300,400,500};
static int yvalue[NUM];
static int scale=1;
static int target = -1;

void drawAxis()
{
	int index,x;
	GUI_RECT Rect;
	char str[20];
	
	GUI_SetColor(WHITE);
	GUI_FillRect(0,120,479,799);
	GUI_SetColor(0x00bbbbbb);
	GUI_SetBkColor(WHITE);
	GUI_SetFont(&GUI_FontHelvetica32);
	
	for(index=0;index<5;index++)
	{
		GUI_DrawVLine(oriY+index*yinterval,oriX,oriX+width-1);
		Rect.x0 = oriY+index*yinterval-16;
		Rect.x1 = oriY+index*yinterval+16;
		Rect.y0 = 120;
		Rect.y1 = 209;
		sprintf(str,"%d",index*500*scale);
		GUI_DispStringInRectEx(str,&Rect,GUI_TA_RIGHT|GUI_TA_VCENTER,strlen(str),GUI_ROTATE_CW);
	}
	
	for(index=0;index<23;index++)
	{
		GUI_DrawHLine(oriX+(int)(log10(xcoordinate[index])*185),oriY,oriY+height-1);
		if(xcoordinate[index]==1 || xcoordinate[index]==10 || xcoordinate[index]==100 || xcoordinate[index]==500)
		{
			Rect.x0 = 0;
			Rect.x1 = 32;
			Rect.y0 = oriX+(int)(log10(xcoordinate[index])*185)-25;
			Rect.y1 = oriX+(int)(log10(xcoordinate[index])*185)+25;
			sprintf(str,"%d",xcoordinate[index]);
			GUI_DispStringInRectEx(str,&Rect,GUI_TA_HCENTER|GUI_TA_BOTTOM,strlen(str),GUI_ROTATE_CW);
		}
	}
}

static void plot(int index)
{
	int x,y,x0,y0;
	
	GUI_SetPenSize(2);
	GUI_SetColor(color);
	x = oriX+(int)(log10(xvalue[index])*185);
	y = oriY+(int)(yvalue[index]/2000.0/scale*height);
	GUI_AA_FillCircle(y,x,2);
	if(index)
	{
		x0 = oriX+(int)(log10(xvalue[index-1])*185);
		y0 = oriY+(int)(yvalue[index-1]/2000.0/scale*height);
		GUI_AA_FillCircle(y0,x0,2);
		GUI_AA_DrawLine(y0,x0,y,x);
	}
}

void scan()
{
	int index;
	int j;
	
	target = -1;
	analog(1);
	scale = 1;
	drawAxis();
	
	for(index=0;index<NUM;index++)
	{
		yvalue[index] = ztest(xvalue[index]);
		
		if(yvalue[index]>2000*scale)
		{
			while(yvalue[index]>2000*scale && scale<128)
				scale *= 2;
			drawAxis();
			for(j=0;j<index;j++)
			{
				plot(j);
			}
		}
		plot(index);
	}
	analog(0);
	UITouchClear();
	widgetList[1].widgetDraw(&widgetList[1]);
}

void select()
{
	int index;
	int x,y;
	char str[30];
	GUI_RECT Rect={479-32,0,479,799};
	
	for(index=0;index<NUM;index++)
	{
		if(getUIInfo()->tpY<=oriX+(int)(log10(xvalue[index])*185))
		{
//			if(target>=0 && target!=index)
//			{
//				GUI_SetColor(color);
//				x = oriX+(int)(log10(xvalue[target])*185);
//				y = oriY+(int)(yvalue[target]/2000.0/scale*height);
//				GUI_AA_FillCircle(y,x,2);
//			}
//			GUI_SetColor(0xe4);
//			x = oriX+(int)(log10(xvalue[index])*185);
//			y = oriY+(int)(yvalue[index]/2000.0/scale*height);
//			GUI_AA_FillCircle(y,x,2);
			sprintf(str,"     %dHz : %d}     ",xvalue[index],yvalue[index]);
			GUI_SetColor(color);
			GUI_DispStringInRectEx(str,&Rect,GUI_TA_HCENTER,strlen(str),GUI_ROTATE_CW);
			target = index;
			break;
		}
	}
}
	


static struct UIWidget widgetList[2] =
{
	{0,1,1,{40,170,439,719},"",0,NULL,NULL,NULL,select},
	{1,1,0,{120,740,359,779},"Scan",0,NULL,NULL,drawButton,scan},
};

static void pageInit(struct UIPage * page)
{
	memset(yvalue,0,4*NUM);
	scale = 1;
	drawAxis();
}
static void pageReturn(struct UIPage * page)
{
	target = -1;
	PageJump(&pMain);
}

struct UIPage pZVF = 
{
	"Impedance Plot",
	2,//char widgetNum;
	-1,
	widgetList,//struct UIWidget * widgetList;
	pageInit,
	pageReturn,
	keyboardEvent,
	touchEvent,
};

