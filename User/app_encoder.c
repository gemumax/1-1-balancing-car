#include "stm32f10x.h"                  // Device header
#include "delay.h"

static volatile int64_t encoder_L = 0;//volatile:在中断要被修改
static volatile int64_t encoder_R = 0;
static volatile int8_t direction_L = 1;//左电机旋转方向；1 -- 正转，-1 -- 反转
static volatile int8_t direction_R = 1;//右电机旋转方向；1 -- 正转，-1 -- 反转
static volatile uint64_t t0_L, t1_L = 0;		//左电机编码器发生变化的时间。单位是us
static volatile uint64_t t0_R, t1_R = 0;		//右电机编码器发生变化的时间。单位是us

static void Encoder_L_Init(void);
static void Encoder_R_Init(void);

//编码器
void App_Encoder_Init(void)
{
	Encoder_L_Init();
	Encoder_R_Init();
}

//读取左轮胎旋转的角速度
float App_Encoder_GetSpeed_L(void)
{
	__disable_irq();
	
	int8_t direction_cpy = direction_L;
	uint64_t t0_cpy = t0_L;
	uint64_t t1_cpy = t1_L;
	
	__enable_irq();
	
	if(direction_cpy == 2 || direction_cpy == -2)
	{
		return 0.0f;
	}
	else
	{
		uint64_t now = GetUs();
		float T;
	
		if((t0_cpy - t1_cpy) > (now - t0_cpy))
		{
			T = (t0_cpy - t1_cpy) * 1.0e-6f;
		}
		else
		{
			T = (now - t0_cpy) * 1.0e-6f;
		}
		return direction_cpy / T / 22.0f / (30613.0f /1500.0f) * 6.2831853f;
	}
}

//读取右轮胎旋转的角速度
float App_Encoder_GetSpeed_R(void)
{
	__disable_irq();
	
	int8_t direction_cpy = direction_R;
	uint64_t t0_cpy = t0_R;
	uint64_t t1_cpy = t1_R;
	
	__enable_irq();
	
	if(direction_cpy == 2 || direction_cpy == -2)
	{
		return 0.0f;
	}
	else
	{
		uint64_t now = GetUs();
		float T;
	
		if((t0_cpy - t1_cpy) > (now - t0_cpy))
		{
			T = (t0_cpy - t1_cpy) * 1.0e-6f;
		}
		else
		{
			T = (now - t0_cpy) * 1.0e-6f;
		}
		return direction_cpy / T / 22.0f / (30613.0f /1500.0f) * 6.2831853f;
	}
}

//左编码器
static void Encoder_L_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);//PB14
	
	EXTI_InitTypeDef EXTI_InitStruct = {0};
	EXTI_InitStruct.EXTI_Line = EXTI_Line14;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode =  EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_Init(&EXTI_InitStruct);
	
	NVIC_InitTypeDef NVIC_InitStruct = {0};
	NVIC_InitStruct.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStruct);
}

static void Encoder_R_Init(void)//右编码器
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//重映射
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource3);//PB3
	
	EXTI_InitTypeDef EXTI_InitStruct = {0};
	EXTI_InitStruct.EXTI_Line = EXTI_Line3;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode =  EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_Init(&EXTI_InitStruct);
	
	NVIC_InitTypeDef NVIC_InitStruct = {0};
	NVIC_InitStruct.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStruct);
}

//左编码器的A相
void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetFlagStatus(EXTI_Line14) == SET)
	{
		EXTI_ClearFlag(EXTI_Line14);
		
		t1_L = t0_L;
		t0_L = GetUs();
	
		uint8_t a = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14);
		uint8_t b = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15);
		
		if((a == Bit_SET && b == Bit_RESET) || (a == Bit_RESET && b == Bit_SET))
		{
			encoder_L--;
			if(direction_L > 0)
			{
				direction_L = -2;
			}
			else
			{
				direction_L = -1;
			}								
		}
		else
		{
			encoder_L++;
			if(direction_L < 0)
			{
				direction_L = 2;
			}
			else
			{
				direction_L = 1;
			}						
		}		
	}	
}

//右编码器的A相
void EXTI3_IRQHandler(void)
{
	EXTI_ClearFlag(EXTI_Line3);
	
	t1_R = t0_R;
	t0_R = GetUs();
	
	uint8_t a = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3);
	uint8_t b = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4);
	
	if((a == Bit_SET && b == Bit_RESET) || (a == Bit_RESET && b == Bit_SET))
		{
			encoder_R++;
			if(direction_R < 0)
			{
				direction_R = 2;
			}
			else
			{
				direction_R = 1;
			}
		}
		else
		{
			encoder_R--;
			if(direction_R > 0)
			{
				direction_R = -2;
			}
			else
			{
				direction_R = -1;
			}
		}	
}

//左轮胎旋转角度
float App_Encoder_GetPos_L(void)
{
	return encoder_L / 22.0f / (30613.0f / 1500.0f) * 360.0f;
}

float App_Encoder_GetPos_R(void)
{
	return encoder_R / 22.0f / (30613.0f / 1500.0f) * 360.0f;
}
