#include "stm32f10x.h"                  // Device header
#include "i2c.h"
#include "delay.h"
#include "task.h"
#include <math.h>
#include "qmath.h"

static float ax, ay, az;//加速度结果
static float gx, gy, gz;
static float temperature;//温度结果
static float yaw, pitch, roll;//z, x, y

static void reg_write(uint8_t reg, uint8_t value);
static uint8_t reg_read(uint8_t reg);

void App_mpu6050_Init(void)
{
	//PB8 - PB9 - IIC
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_I2C1, ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);
	
	I2C_InitTypeDef I2C_InitStruct = {0};
	I2C_InitStruct.I2C_ClockSpeed = 400000;//IIC通信速度
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;//占空比
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_Init(I2C1,&I2C_InitStruct);	
	
	
	reg_write(0x6b, 0x80);
	Delay(100);
	reg_write(0x6b, 0x00);
	reg_write(0x1b, 0x18);
	reg_write(0x1c, 0x00);
	
}

//更新6050的值
void App_mpu6050_Update(void)
{
	int16_t ax_raw = (int16_t)((reg_read(0x3b) << 8) + reg_read(0x3c));
	int16_t ay_raw = (int16_t)((reg_read(0x3d) << 8) + reg_read(0x3e));
	int16_t az_raw = (int16_t)((reg_read(0x3f) << 8) + reg_read(0x40));
	
	ax = ax_raw * 6.1035e-5f;
	ay = ay_raw * 6.1035e-5f;
	az = az_raw * 6.1035e-5f;
	
	int16_t temperature_raw = (int16_t)((reg_read(0x41) << 8) + reg_read(0x42));
	
	temperature = temperature_raw / 338.87f + 21.0f;
	
	int16_t gx_raw = (int16_t)((reg_read(0x43) << 8) + reg_read(0x44));
	int16_t gy_raw = (int16_t)((reg_read(0x45) << 8) + reg_read(0x46));
	int16_t gz_raw = (int16_t)((reg_read(0x47) << 8) + reg_read(0x48));
	
	gx = gx_raw * 6.1035e-2f;
	gy = gy_raw * 6.1035e-2f;
	gz = gz_raw * 6.1035e-2f;
}

//进程函数，计算欧拉角
void App_mpu6050_Proc(void)
{
	PERIODIC(5)  //每5ms进行一次
	
	App_mpu6050_Update();//更新传感器的值
	
	//陀螺仪计算
	float yaw_g = yaw + gz * 0.005;
	float pitch_g = pitch + gx * 0.005;
	float roll_g = roll - gy * 0.005;
	
	//加速度计算
	float pitch_a = qatan2(ay, az) / 3.1415927f * 180.0f;
	float roll_a = qatan2(ax, az) / 3.1415927f * 180.0f;
	
	//互补滤波器计算
	yaw = yaw_g;
	pitch = 0.95238 * pitch_g + (1 - 0.95238) * pitch_a;
	roll = 0.95238 * roll_g + (1 - 0.95238) * roll_a;
}

float App_mpu6050_GetAx(void)
{
	return ax;
}

float App_mpu6050_GetAy(void)
{
	return ay;
}

float App_mpu6050_GetAz(void)
{
	return az;
}

float App_mpu6050_GetTemperature(void)
{
	return temperature;
}

float App_mpu6050_GetGx(void)
{
	return gx;
}

float App_mpu6050_GetGy(void)
{
	return gy;
}

float App_mpu6050_GetGz(void)
{
	return gz;
}

float App_mpu6050_Getyaw(void)
{
	return yaw;
}

float App_mpu6050_Getpitch(void)
{
	return pitch;
}

float App_mpu6050_Getroll(void)
{
	return roll;
}

//向寄存器写值
//reg - 寄存器地址
static void reg_write(uint8_t reg, uint8_t value)
{
	uint8_t bytesToSend[] = {reg, value};
	
	My_I2C_SendBytes(I2C1, 0xd0, bytesToSend, 2);
}

static uint8_t reg_read(uint8_t reg)
{
	My_I2C_SendBytes(I2C1, 0xd0, &reg, 1);//发送寄存器地址
	
	uint8_t regValue;
	My_I2C_ReceiveBytes(I2C1, 0xd0, &regValue,1);
	return regValue;
}
