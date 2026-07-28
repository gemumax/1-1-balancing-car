#include "stm32f10x.h"                  // Device header
#include "button.h"
#include "app_PWM.h"
#include "app_Motor.h"
#include "app_control.h"

static Button_TypeDef userKey;
static void OnUserKey_Clicked(uint8_t clicks);

void App_Button_Init(void)
{
	Button_InitTypeDef Button_InistStruct = {0};
	Button_InistStruct.GPIOx = GPIOA;
	Button_InistStruct.GPIO_Pin = GPIO_Pin_11;
	My_Button_Init(&userKey, &Button_InistStruct);
	
	My_Button_SetClickCb(&userKey,OnUserKey_Clicked);
}

void App_Button_Proc(void)
{
	My_Button_Proc(&userKey);
}

static uint8_t pwm_on = 0;                        //pwm_cmd

//按钮点击的回调函数
static void OnUserKey_Clicked(uint8_t clicks)
{
	if(clicks == 1)
	{
		App_Control_Reset();
		if(pwm_on ==0)
		{
			pwm_on = 1;
		}
		else
		{
			pwm_on = 0;
		}
	}
	App_Motor_Cmd(pwm_on);
}
