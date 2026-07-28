#include "stm32f10x.h"                  // Device header
#include "math.h"

static void STBY_Pin_Init(void);
static void Motor_L_Init(void);//左电机
static void Motor_R_Init(void);//右电机

void App_PWM_Init(void)
{
	STBY_Pin_Init();
	Motor_L_Init();
	Motor_R_Init();
}

//PA1       开启H桥引脚
void STBY_Pin_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
}

//控制TB6612进入活动或休眠
//0 - 休眠
//非零 - 活动
void App_PWM_Cmd(uint8_t on)
{
	if(on == 0)
	{
		GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_RESET);
	}
	else
	{
		GPIO_WriteBit(GPIOA, GPIO_Pin_1, Bit_SET);
	}
}

//左电机占空比
//duty = -100到100
void App_PWM_Set_L(float Duty)
{
	float slgn;
	if(Duty >= 0)slgn = 1;
	else slgn = -1;
	
	Duty = fabsf(Duty);  //取绝对值
	
	if(slgn < 0)  //正转
	{
		GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_SET);
		GPIO_WriteBit(GPIOA, GPIO_Pin_10, Bit_RESET);
	}
	else       		//反转
	{
		GPIO_WriteBit(GPIOA, GPIO_Pin_9, Bit_RESET);
		GPIO_WriteBit(GPIOA, GPIO_Pin_10, Bit_SET);
	}
	uint16_t ccr = Duty / 100.0f * 999;
	
	TIM_SetCompare1(TIM1, ccr);//设置捕获寄存器的值
}

////右电机占空比
void App_PWM_Set_R(float Duty)
{
	float slgn;
	if(Duty >= 0)slgn = 1;
	else slgn = -1;
	
	Duty = fabsf(Duty);  //取绝对值
	
	if(slgn >= 0)  //正转
	{                     
		GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_SET);
		GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_RESET);
	}
	else       		//反转
	{
		GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_RESET);
		GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_SET);
	}
	uint16_t ccr = Duty / 100.0f * 999;
	
	TIM_SetCompare1(TIM4, ccr);//设置捕获寄存器的值
}

static void Motor_L_Init(void)//左电机
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	//时基单元
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct = {0};
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 999;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 0;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStruct);
	
	//配置输出比较
	TIM_OCInitTypeDef TIM_OCInitStruct = {0};
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_OutputState = ENABLE;
	TIM_OCInitStruct.TIM_Pulse = 0;
	TIM_OC1Init(TIM1,&TIM_OCInitStruct);
	
	TIM_CtrlPWMOutputs(TIM1,ENABLE);
	
	TIM_Cmd(TIM1,ENABLE);
}

static void Motor_R_Init(void)//右电机
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct = {0};
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 999;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 0;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStruct);
	
	//配置输出比较
	TIM_OCInitTypeDef TIM_OCInitStruct = {0};
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_OutputState = ENABLE;
	TIM_OCInitStruct.TIM_Pulse = 0;
	TIM_OC1Init(TIM4,&TIM_OCInitStruct);
	
	TIM_CtrlPWMOutputs(TIM4,ENABLE);
	
	TIM_Cmd(TIM4,ENABLE);
}
