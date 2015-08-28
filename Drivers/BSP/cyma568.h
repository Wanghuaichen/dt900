#ifndef __CYMA568_H
#define __CYMA568_H
#include "stm32f4xx_hal.h"
void tpTest();
void ping();
void touchreport();
void HidDes();
void sysinfo();
int CalibrateIDAC();
int SuspendScan();
int ResumeScan();
int ShortTest();
int OpenTest();
void tpScan();
#endif