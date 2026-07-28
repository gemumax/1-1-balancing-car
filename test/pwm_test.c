#include "stm32f10x.h"                  // Device header
#include "app_pwm.h"
#include "delay.h"


void PWM_Test(void)
{
	App_PWM_Init();
	
	App_PWM_Cmd(1);
	
	App_PWM_Set_L(30);//30%占空比
	App_PWM_Set_R(30);
	Delay(2000);
	
	App_PWM_Set_L(60);//60%占空比
	App_PWM_Set_R(60);
	Delay(2000);
	
	App_PWM_Set_L(90);//90%占空比
	App_PWM_Set_R(90);
	Delay(2000);
	
	App_PWM_Cmd(0);
	
	
	while(1)
	{
		
	}
}
