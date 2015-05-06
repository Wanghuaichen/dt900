#ifndef __LCD_H
#define __LCD_H

#include "stm32f4xx_hal.h"

extern uint32_t FB_ADDR[2];

void LCD_PWR(int val);
void LCD_BL(int val);
void LCD_LayerCfg(void);

#endif
