#include "ds3231m.h"
#include "rtc.h"
#include "ui.h"
#define I2C_ADDR 0xD0

I2C_HandleTypeDef hi2c2;
uint8_t data[8];
uint8_t RTC_year,RTC_month,RTC_date,RTC_hour,RTC_minute,RTC_second;

static int sendReport(uint8_t* report,int size)
{
	if(HAL_I2C_Master_Transmit(&hi2c2,I2C_ADDR,report,size,200)!=HAL_OK)
		return -1;
	return 0;
}

static int getReport(uint8_t* report,int size)
{
	if(HAL_I2C_Master_Receive(&hi2c2,I2C_ADDR,report,size,200)!=HAL_OK)
		return -1;
	return 0;
}

uint8_t BIN2BCD(uint8_t num)
{
	return ((num/10)<<4)+num%10;
}

uint8_t BCD2BIN(uint8_t num)
{
	return (num>>4)*10+(num&0xf);
}
	
void DS3231M_OsciCheck()
{
	data[0] = 0x0F;
	sendReport(data,1);
	getReport(data,1);
	if(data[0]&0x80)
	{
		data[0]=0xE;
		data[1]=0x4;
		data[2]=0x0;
		sendReport(data,3);
	}
}
	

void DS3231M_Init()
{
	hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 400000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0x0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLED;
  HAL_I2C_Init(&hi2c2);
	HAL_I2CEx_AnalogFilter_Config (&hi2c2,I2C_ANALOGFILTER_ENABLED);
}


void DS3231M_SetTime(RTC_DateTypeDef *rtcDate,RTC_TimeTypeDef *rtcTime)
{
	data[0] = 0x0;
	data[1] = BIN2BCD(rtcTime->Seconds);
	data[2] = BIN2BCD(rtcTime->Minutes);
	data[3] = BIN2BCD(rtcTime->Hours);
	data[4] = 0x1;
	data[5] = BIN2BCD(rtcDate->Date);
	data[6] = BIN2BCD(rtcDate->Month);
	data[7] = BIN2BCD(rtcDate->Year);
	sendReport(data,8);
}
	
void DS3231M_GetTime(RTC_DateTypeDef *rtcDate,RTC_TimeTypeDef *rtcTime)
{
	data[0]=0x0;
	sendReport(data,1);
	getReport(data,7);
	rtcTime->Seconds = BCD2BIN(data[0]);
	rtcTime->Minutes = BCD2BIN(data[1]);
	rtcTime->Hours = BCD2BIN(data[2]);
	rtcDate->Date = BCD2BIN(data[4]);
	rtcDate->Month = BCD2BIN(data[5]);
	rtcDate->Year = BCD2BIN(data[6]);
}

float DS3231M_GetTemp()
{
	data[0] = 0x11;
	sendReport(data,1);
	getReport(data,2);
	return (char)data[0]+(data[1]>>6)*0.25;
}


