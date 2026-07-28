#include "stm32f10x.h"                  // Device header
#include "pid.h"
#include "task.h"
#include "app_encoder.h"
#include "app_bat.h"
#include "app_pwm.h"

static PID_TypeDef PID_Motor_L;//左电机调速系统的PID控制器
static PID_TypeDef PID_Motor_R;//右电机调速系统的PID控制器

void App_Motor_Init(void)
{
	PID_Init(&PID_Motor_L, 0.5, 7, 0);
	PID_LimitConfig(&PID_Motor_L,+8.4, -8.4);
	PID_Init(&PID_Motor_R, 0.5, 7, 0);
	PID_LimitConfig(&PID_Motor_R,+8.4, -8.4);
}

void App_Motor_Proc(void)
{
	PERIODIC(1)
	
	//获取左右电机旋转的角速度
	float omega_L = App_Encoder_GetSpeed_L();
	float omega_R = App_Encoder_GetSpeed_R();
	
	//计算PID控制器的输出
	float Ua_L = PID_Compute(&PID_Motor_L, omega_L);
	float Ua_R = PID_Compute(&PID_Motor_R, omega_R);
	
	//讲电压Ua设置到电机两端
	//获取电池电压Vbat
	float Vbat = App_Bat_Get();
	
	//计算占空比
	float duty_L = Ua_L / Vbat * 100.0f;
	float duty_R = Ua_R / Vbat * 100.0f;
	
	App_PWM_Set_L(duty_L);
	App_PWM_Set_R(duty_R);
}

//设置电机转速
//单位：rad/s
void App_Motor_SetOmega_L(float Omega)
{
	PID_ChangeSP(&PID_Motor_L, Omega);
}

void App_Motor_SetOmega_R(float Omega)
{
	PID_ChangeSP(&PID_Motor_R, Omega);
}

//开关电机
void App_Motor_Cmd(uint8_t on)
{
	App_PWM_Cmd(on);
	PID_Reset(&PID_Motor_L);
	PID_Reset(&PID_Motor_R);
}
