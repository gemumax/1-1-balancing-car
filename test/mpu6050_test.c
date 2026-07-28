#include "stm32f10x.h"                  // Device header
#include "app_usart.h"
#include "app_mpu6050.h"
#include "delay.h"
#include "task.h"

void MPU6050_Tste(void)
{
	App_USART2_Init();
	App_mpu6050_Init();
	
	while(1)
	{
		App_mpu6050_Update();
		float ax = App_mpu6050_GetAx();
		float ay = App_mpu6050_GetAy();
		float az = App_mpu6050_GetAz();
		
		float temperature = App_mpu6050_GetTemperature();
		
		float gx = App_mpu6050_GetGx();
		float gy = App_mpu6050_GetGy();
		float gz = App_mpu6050_GetGz();
		
		My_USART_Printf(USART2, "%f,%f,%f,%f,%f,%f,%f\n", ax,ay,az, temperature, gx,gy,gz);
		Delay(10);
	}
}

static void USART2_Proc(void);

//测试欧拉角
void MPU6050_EularAngleTest(void)
{
	App_USART2_Init();
	App_mpu6050_Init();
	
	while(1)
	{
		App_mpu6050_Proc();
		USART2_Proc();
	}
}

//每间隔10ms发送一次
static void USART2_Proc(void)
{
	PERIODIC(10)
	
	float ax = App_mpu6050_GetAx();
	float ay = App_mpu6050_GetAy();
	float az = App_mpu6050_GetAz();
		
	float temperature = App_mpu6050_GetTemperature();
	
	float gx = App_mpu6050_GetGx();
	float gy = App_mpu6050_GetGy();
	float gz = App_mpu6050_GetGz();
	
	float yaw = App_mpu6050_Getyaw();
	float pitch = App_mpu6050_Getpitch();
	float roll = App_mpu6050_Getroll();
	
	My_USART_Printf(USART2, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", ax,ay,az, temperature, gx,gy,gz, yaw, pitch, roll);
}
