#include "stm32f10x.h"                  // Device header
#include "string.h"
#include "stdio.h"
#include "app_control.h"

#define CMD_MAX_LEN 64

static char intBuf[CMD_MAX_LEN];
static char transBuf[CMD_MAX_LEN];
static char procBuf[CMD_MAX_LEN];

static volatile uint8_t lineReceivedFlag = 0;
static uint16_t intBufCursor = 0;

void App_RC_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	
	USART_InitTypeDef USART_InitStruct = {0};
	USART_InitStruct.USART_BaudRate = 9600;
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART3, &USART_InitStruct);
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	
	NVIC_InitTypeDef NVIC_InitStruct = {0};
	NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStruct);
	
	USART_Cmd(USART3,ENABLE);
}

void App_RC_Proc(void)
{
	if(lineReceivedFlag)
	{
		strcpy(procBuf,transBuf);
		lineReceivedFlag = 0;
		
		if(strncasecmp(procBuf, "move ", 5) == 0)
		{
			int turnSpeed, moveSpeed;
			
			if(sscanf(procBuf, "move %d %d", &turnSpeed, &moveSpeed) == 2)
			{
				App_Control_SetMoveSpeed(-moveSpeed * 0.01f * 0.7f);
				App_Control_SetTurnSpeed(-turnSpeed * 0.01f * 15.0f);
			}
		}
	}
}

void USART3_IRQHandler(void)
{
	if(USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == SET)
	{
		uint8_t data = USART_ReceiveData(USART3);
		
		if(data != '\n')
		{
			intBuf[intBufCursor++] = data;
		}
		else
		{
			intBuf[intBufCursor] = '\0';
			intBufCursor = 0;
			strcpy(transBuf, intBuf);
			lineReceivedFlag = 1;
		}
	
	}
}
