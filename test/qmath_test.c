#include "stm32f10x.h"                  // Device header
#include "app_usart.h"
#include "delay.h"

void Qmath_Test(void)
{
	App_USART2_Init();
	
	My_USART_Printf(USART2, "\nStart testing integer addition speed:");
	
	int a = 1, b = 2, c;
	
	uint32_t t1 = GetTick();
	
	for(uint32_t i = 0; i < 1000000; i++)
	{
		c = a + b;
	}
	uint32_t t2 = GetTick();
	
	My_USART_Printf(USART2,"Result:%.3fus\n",(t2 - t1) / 1000000.0f * 1000.0f);
	
	while(1)
	{
		
	}
}
