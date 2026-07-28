#include "stm32f10x.h"                  // Device header
#include "app_usart.h"
#include "app_bat.h"
#include "delay.h"
#include "usart.h"

//电池电压检测
void Bat_Test(void)
{
	App_USART2_Init();
	App_Bat_Init();
	
	while(1)
	{
		float Volt = App_Bat_Get();
		My_USART_Printf(USART2, "%.3f\n", Volt);
		Delay(10); //每隔10ms显示一个数据点
		
		
	}
}




