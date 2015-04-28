#ifndef __LCD_H
#define __LCD_H

#include "stm32f4xx_hal.h"

#define FB_LAYER1	((uint32_t)0xD0000000)
#define FB_LAYER2	((uint32_t)0xD0200000)

void LCD_PWR(int val);
void LCD_BL(int val);

#endif
