#include "ui.h"
#include "geotest.h"

extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelvetica32;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontHelveticaNeueLT48;
extern struct UIInfo UIInfo;
extern struct GeoParam geoparam[10];
extern struct Settings settings;

struct UIPage pKeyboard;

struct KBInfo kbInfo;
static char StringArray[10][5]={"","ABC","DEF","GHI","JKL","MNO","PQRS","TUV","WXYZ","-"};


static void keyPress(struct UIWidget * widget)
{
	char str[20];
	int index;
	int len;
	
	len = strlen(kbInfo.kbBuff);
	if(len>=16)
		return;
	if(UIInfo.keyCombo==0)
		len++;
	index = UIInfo.keyCombo/50%widget->widgetValue;
	if(index==0)
		kbInfo.kbBuff[len-1] = *widget->widgetTitle;
	else
		kbInfo.kbBuff[len-1] = ((char*)(widget->widgetPtr))[index-1];
	kbInfo.kbBuff[len] = '\0';
	sprintf(str,"  %s_  ",kbInfo.kbBuff);
	GUI_SetColor(BLACK);
	GUI_SetBkColor(WHITE);
	GUI_SetFont(&GUI_FontHelveticaNeueLT48);
	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_VCENTER);
	GUI_DispStringAt(str,240,240);
}

static void keyDel(struct UIWidget * widget)
{
	char str[30];
	int len;
	len = strlen(kbInfo.kbBuff);
	if(len==0)
		return;
	kbInfo.kbBuff[len-1] = '\0';
	sprintf(str,"   %s_   ",kbInfo.kbBuff);
	GUI_SetColor(BLACK);
	GUI_SetBkColor(WHITE);
	GUI_SetFont(&GUI_FontHelveticaNeueLT48);
	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_VCENTER);
	GUI_DispStringAt(str,240,240);
}

static void drawKey(struct UIWidget * widget)
{
	GUI_SetColor(WHITE);
	GUI_FillRect(widget->rect.x0,widget->rect.y0,widget->rect.x1,widget->rect.y1);	
	GUI_SetPenSize(2);
	GUI_SetColor(widget->active ? 0x003fceff : 0x002f2f2f);
	GUI_AA_DrawRoundedRect(widget->rect.x0+1,widget->rect.y0+1,widget->rect.x1-1,widget->rect.y1-1,
	(widget->rect.y1-widget->rect.y0-2)/4);
	
	GUI_SetColor(widget->active ? 0x002fbeff : BLACK);
	GUI_SetBkColor(WHITE);
	
	if(widget->widgetIndex==9 || widget->widgetIndex==11)
	{
		GUI_SetFont(&GUI_FontHelveticaNeueLT48);	
		GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_VCENTER);
		GUI_DispStringAt(widget->widgetTitle,(widget->rect.x0+widget->rect.x1)/2,(widget->rect.y1+widget->rect.y0)/2);
	}
	else
	{
		GUI_SetFont(&GUI_FontHelveticaNeueLT48);	
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
		GUI_DispStringAt(widget->widgetTitle,widget->rect.x0+15,(widget->rect.y1+widget->rect.y0)/2);
		GUI_SetFont(&GUI_FontHelvetica32);	
		GUI_SetTextAlign(GUI_TA_LEFT | GUI_TA_VCENTER);
		GUI_DispStringAt(widget->widgetPtr,widget->rect.x0+50,(widget->rect.y1+widget->rect.y0)/2);
	}
}

static struct UIWidget widgetList[12] =
{
	{0,1,0,{20,375,159,454},"1",1,StringArray[0],NULL,drawKey,keyPress},
	{1,1,0,{170,375,309,454},"2 ",4,StringArray[1],NULL,drawKey,keyPress},
	{2,1,0,{320,375,459,454},"3",4,StringArray[2],NULL,drawKey,keyPress},
	{3,1,0,{20,485,159,564},"4",4,StringArray[3],NULL,drawKey,keyPress},
	{4,1,0,{170,485,309,564},"5",4,StringArray[4],NULL,drawKey,keyPress},
	{5,1,0,{320,485,459,564},"6",4,StringArray[5],NULL,drawKey,keyPress},
	{6,1,0,{20,595,159,674},"7",5,StringArray[6],NULL,drawKey,keyPress},
	{7,1,0,{170,595,309,674},"8",4,StringArray[7],NULL,drawKey,keyPress},
	{8,1,0,{320,595,459,674},"9",5,StringArray[8],NULL,drawKey,keyPress},
	{9,1,0,{20,705,159,784},".",1,NULL,NULL,drawKey,keyPress},
	{10,1,0,{170,705,309,784},"0",2,StringArray[9],NULL,drawKey,keyPress},
	{11,1,0,{320,705,459,784},"X",0,NULL,NULL,drawKey,keyDel},
};

static void pageInit()
{
	char str[20];
	
	strcpy(pKeyboard.pageTitle,kbInfo.kbTitle);
	sprintf(str,"%s_",kbInfo.kbBuff);
	GUI_SetColor(BLACK);
	GUI_SetBkColor(WHITE);
	GUI_SetFont(&GUI_FontHelveticaNeueLT48);
	GUI_SetTextAlign(GUI_TA_HCENTER | GUI_TA_VCENTER);
	GUI_DispStringAt(str,240,240);
}

static void pageReturn(struct UIPage * page)
{
	if(kbInfo.kbCallBack)
		kbInfo.kbCallBack();
	PageJump(kbInfo.kbParent);
}

struct UIPage pKeyboard = 
{
	"",
	12,//char widgetNum;
	-1,
	widgetList,//struct UIWidget * widgetList;
	pageInit,
	pageReturn,
	keyboardEvent,
	touchEvent,
};

