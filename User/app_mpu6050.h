#ifndef __APP_MPU6050_H
#define __APP_MPU6050_H

void App_mpu6050_Init(void);
void App_mpu6050_Update(void);
void App_mpu6050_Proc(void);

float App_mpu6050_GetAx(void);
float App_mpu6050_GetAy(void);
float App_mpu6050_GetAz(void);

float App_mpu6050_GetTemperature(void);

float App_mpu6050_GetGx(void);
float App_mpu6050_GetGy(void);
float App_mpu6050_GetGz(void);

float App_mpu6050_Getyaw(void);
float App_mpu6050_Getpitch(void);
float App_mpu6050_Getroll(void);

#endif
