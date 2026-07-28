#ifndef APP_BAT_H
#define APP_BAT_H

#include "stm32f10x.h"                  // Device header
void App_Bat_Init(void);
float App_Bat_Get(void);
void App_Bat_proc(void);

#endif
