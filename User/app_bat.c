#include "app_bat.h"                  // Device header
#include "delay.h"

static volatile float Vbat = 0.0f;

static void TIM2_TRGO_Init(void);
static void ADC1_Init(void);
static void LED_Init(void);

//电池电压检测模块               
void App_Bat_Init(void)
{
    TIM2_TRGO_Init();
    ADC1_Init();
    LED_Init();
}

static uint32_t lastTime = 0;//上次LED切换的时间
static uint8_t stage = 0;   //当前LED的状态， 0——熄灭，1——点亮

//电池电压监控模块的任务切片
void App_Bat_proc(void)
{
    if(Vbat > 7.9) //100%点亮3颗
    {
        GPIO_WriteBit(GPIOA, GPIO_Pin_4,Bit_SET);
        GPIO_WriteBit(GPIOA, GPIO_Pin_5,Bit_SET);
        GPIO_WriteBit(GPIOA, GPIO_Pin_6,Bit_SET);
    }
    else if(Vbat > 7.4) //75%点亮2颗
    {
        GPIO_WriteBit(GPIOA, GPIO_Pin_4,Bit_SET);
        GPIO_WriteBit(GPIOA, GPIO_Pin_5,Bit_SET);
        GPIO_WriteBit(GPIOA, GPIO_Pin_6,Bit_RESET);
    }
    else if(Vbat > 7) //50%点亮1颗
    {
        GPIO_WriteBit(GPIOA, GPIO_Pin_4,Bit_SET);
        GPIO_WriteBit(GPIOA, GPIO_Pin_5,Bit_RESET);
        GPIO_WriteBit(GPIOA, GPIO_Pin_6,Bit_RESET);
    }
    else if(Vbat > 6.5)//2%全部熄灭
    {
        GPIO_WriteBit(GPIOA, GPIO_Pin_4,Bit_RESET);
        GPIO_WriteBit(GPIOA, GPIO_Pin_5,Bit_RESET);
        GPIO_WriteBit(GPIOA, GPIO_Pin_6,Bit_RESET);
    }
    else                   //闪烁
    {
        uint32_t now = GetTick();//获取当前时间
    
		if(stage == 0 && now - lastTime > 100)
		{
        GPIO_WriteBit(GPIOA, GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6, Bit_SET);//点亮
        stage =1;
        lastTime = now;
		}
		else if(stage == 1 && now - lastTime > 100)
		{
        GPIO_WriteBit(GPIOA, GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6, Bit_RESET);//熄灭
        stage =0;
        lastTime = now;
		}
    }
}

static void TIM2_TRGO_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE); 
    
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct = {0};
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period = 10000-1;        //ARR
    TIM_TimeBaseInitStruct.TIM_Prescaler = 72-1;           //PSC
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
    
    TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);//主机模式为update
    
    TIM_Cmd(TIM2, ENABLE);
}

static void ADC1_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
    
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);                   //将ADC设置为6分频
    
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;          //模拟模式
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    ADC_InitTypeDef ADC_InitStruct = {0};
    ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStruct.ADC_Mode = DISABLE;
    ADC_InitStruct.ADC_NbrOfChannel = 1;
    ADC_InitStruct.ADC_ScanConvMode = DISABLE;
    ADC_Init(ADC1,&ADC_InitStruct);
    
    //设置注入序列
    ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_T2_TRGO);//TIM2作为TRGO外部触发
    ADC_ExternalTrigInjectedConvCmd(ADC1,ENABLE);
    ADC_InjectedChannelConfig(ADC1, ADC_Channel_8, 1,ADC_SampleTime_1Cycles5);//存疑，，，后续可试试改为7.5
    
    //配置ADC的JEOC中断
    ADC_ITConfig(ADC1, ADC_IT_JEOC,ENABLE);
    
    NVIC_InitTypeDef NVIC_InitStruct = {0};
    NVIC_InitStruct.NVIC_IRQChannel = ADC1_2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
    NVIC_Init(&NVIC_InitStruct);
    
    ADC_Cmd(ADC1,ENABLE);
}

//电源指示灯
static void LED_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStruct ={0};
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
}

float App_Bat_Get(void)
{
    return Vbat;
}

void ADC1_2_IRQHandler(void)
{
    if(ADC_GetFlagStatus(ADC1, ADC_FLAG_JEOC) == SET)
    {
        ADC_ClearFlag(ADC1, ADC_FLAG_JEOC);                 //清除标志位
        uint16_t jdr1 = ADC_GetInjectedConversionValue(ADC1,ADC_InjectedChannel_1);
        
        Vbat = jdr1 / 4095.0f *3.3 * 8.4f / 3.3;
    }
}
