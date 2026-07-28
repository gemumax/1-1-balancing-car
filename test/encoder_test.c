#include "stm32f10x.h"                  // Device header
#include "app_usart.h"
#include "delay.h"
#include "app_encoder.h"
#include "usart.h"

//编码器
void encoder_Test(void)
{
	
	App_USART2_Init();
	App_Encoder_Init();
	
	while(1)
	{
		float pos_L = App_Encoder_GetPos_L();
		float pos_R = App_Encoder_GetPos_R();
		
		My_USART_Printf(USART2, "%f,%f\n", pos_L, pos_R);
		
		Delay(50);
	}
}

static float last_pos_L = 0.0f;
static float last_pos_R = 0.0f;

//M法测速
void encoder_M_Method_Test(void)
{
	App_USART2_Init();
	App_Encoder_Init();
	
	while(1)
	{
		Delay(1);
		
		float pos_L = App_Encoder_GetPos_L();
		float pos_R = App_Encoder_GetPos_R();
		
		float M_L = pos_L - last_pos_L;
		float M_R = pos_R - last_pos_R;
		
		float omega_L = M_L / 0.001f;
		float omega_R = M_R / 0.001f;
		
		My_USART_Printf(USART2, "%f,%f,%f,%f\n", pos_L, pos_R,omega_L,omega_R);
		
		last_pos_L = pos_L;
		last_pos_R = pos_R;
	}
}

void encoder_T_Method_Test(void)
{
	App_USART2_Init();
	App_Encoder_Init();
	
	while(1)
	{
		Delay(1);
		
		float omega_L = App_Encoder_GetSpeed_L();
		float omega_R = App_Encoder_GetSpeed_R();
		
		My_USART_Printf(USART2, "%f,%f\n",omega_L,omega_R);
	}
}
