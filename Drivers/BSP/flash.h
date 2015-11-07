#ifndef __FLASH_H
#define __FLASH_H

#include "stm32f4xx_hal.h"

//128k
#define SETADDR 0x081F0000
#define PARAMADDR 0x081E0000
#define PASSADDR 0x081C0000
void FlashProgram();
void FlashEraseTotal24();
void FlashPasswd(long num);
#endif

