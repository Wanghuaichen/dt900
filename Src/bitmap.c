#include "ff_gen_drv.h"
#include "beep.h"
#include "ui.h"
#include "rtc.h"

extern IWDG_HandleTypeDef IwdgHandle;
extern RTC_DateTypeDef rtcDate;
extern RTC_TimeTypeDef rtcTime;

typedef unsigned char BYTE;               // 1B  
typedef unsigned short WORD;           // 2B  
typedef unsigned long DWORD;          // 4B  
typedef long LONG;                            // 4B  

typedef struct tagBITMAPFILEHEADER {
	WORD paddings;
	WORD magic;
	DWORD bfSize;
	WORD bfReserved1;
	WORD bfReserved2;
	DWORD bfOffBits;
}BITMAPFILEHEADER;  

typedef struct tagBITMAPINFOHEADER{  
	DWORD biSize;
	LONG biWidth;
	LONG biHeight;
	WORD biPlanes;
	WORD biBitCount;
	DWORD biCompression;
	DWORD biSizeImage;
	LONG biXPelsPerMeter;
	LONG biYPelsPerMeter;
	DWORD biClrUsed;
	DWORD biClrImportant;
}BITMAPINFOHEADER;  

void SaveBitmap()
{
	FIL MyFile;
	UINT wbytes;
	char str[20];
	BITMAPFILEHEADER fileheader;
	BITMAPINFOHEADER infoheader;
	
	fileheader.magic = 0x4D42;
	fileheader.bfOffBits = 54;
	fileheader.bfSize = 54+800*480*3;
	fileheader.bfReserved1 = 0;
	fileheader.bfReserved2 = 0;
	
	infoheader.biSize = 40;
	infoheader.biWidth = 800;
	infoheader.biHeight = 480;
	infoheader.biPlanes = 1;
	infoheader.biBitCount = 24;
	infoheader.biCompression = 0;
	infoheader.biSizeImage = 800*480*3;
	infoheader.biXPelsPerMeter = 0;
	infoheader.biYPelsPerMeter = 0;
	infoheader.biClrUsed = 0;
	infoheader.biClrImportant = 0;

beep(500);
GUI_SetColor(BLACK);
GUI_DrawRect(0,0,479,799);

	sprintf(str,"%02d%02d%02d.bmp",rtcTime.Hours, rtcTime.Minutes,rtcTime.Seconds);
	if(f_open(&MyFile,(TCHAR *)str,FA_OPEN_ALWAYS | FA_WRITE) != FR_OK)
		return;
	f_write(&MyFile, (char *)(&fileheader)+2, 14, (void *)&wbytes);
	f_write(&MyFile, &infoheader, 40, (void *)&wbytes);
	f_write(&MyFile, (char *)0xD0000000, 800*480, (void *)&wbytes);
	HAL_IWDG_Refresh(&IwdgHandle);
	f_write(&MyFile, (char *)0xD0000000+800*480, 800*480, (void *)&wbytes);
	HAL_IWDG_Refresh(&IwdgHandle);
	f_write(&MyFile, (char *)0xD0000000+800*480*2, 800*480, (void *)&wbytes);
	HAL_IWDG_Refresh(&IwdgHandle);
	f_close(&MyFile);
	beep(1000);
}
