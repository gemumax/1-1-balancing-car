#include "stm32f10x.h"                  // Device header
#include "pid.h"
#include "task.h"
#include "app_mpu6050.h"
#include "qmath.h"
#include "delay.h"
#include "app_motor.h"
#include "app_encoder.h"

static PID_TypeDef pid_velocity;
static PID_TypeDef pid_theta;//θ；环的pid控制器
static PID_TypeDef pid_theta_dot;//角速度的pid控制器
static PID_TypeDef pid_turn;

static const float g = 9.8;
static const float lp = 0.062;
static const float rw = 0.032;

void App_Control_Init(void)
{
	PID_Init(&pid_velocity, 10.0f, 1.0f, 0.0f);
	PID_LimitConfig(&pid_velocity, +0.5*g, -0.5*g);
	
	PID_Init(&pid_theta, 4.0f, 0.0f, 0.0f);
	PID_LimitConfig(&pid_theta, +12.57f, -12.57f);
	
	PID_Init(&pid_theta_dot, 10.0f, 10.0f, 0.0f);
	PID_LimitConfig(&pid_theta_dot, +125.7f, -125.7f);
	
	PID_Init(&pid_turn, 1.0f, 0.0f, 0.0f);
	PID_LimitConfig(&pid_turn, +15.0f, -15.0f);
}

static float omega_ref = 0.0f;
static uint64_t lastTime = 0;

void App_Control_Proc(void)
{
	PERIODIC(5)
	
	uint64_t now = GetUs(); // 获取微秒级时间（原为GetTick返回毫秒，导致deltaT偏小1000倍）
	float deltaT = (now - lastTime) * 1.0e-6f;
	
	//PID_ChangeSP(&pid_velocity, 0.0f);
	
	float omega = 0.5 * (App_Encoder_GetSpeed_L() + App_Encoder_GetSpeed_R());
	
	//读取传感器的数据
	float theta = App_mpu6050_Getpitch() * 0.0174533;
	float theta_dot = App_mpu6050_GetGx() * 0.0174533;//单位：rad/s
	
	float omega2 = -theta_dot * (lp + rw) / rw;
	float omega1 = omega - omega2;
	float x_dot = omega1 * rw;
	
	float theta_ref = qatan(PID_Compute(&pid_velocity, x_dot) / g);
	
	//将外环的设定值SP设置为0
	PID_ChangeSP(&pid_theta, theta_ref);
	
	
	//计算外环pid输出
	float theta_dot_ref = PID_Compute(&pid_theta, theta);
	
	//改变内环PID的设定值（原此处缺失ChangeSP，外环输出无法传递到内环，已补上）
	PID_ChangeSP(&pid_theta_dot, theta_dot_ref);
	
	//计算内环PID输出
	float theta_dot_dot_ref = PID_Compute(&pid_theta_dot, theta_dot);
	
	//倒立摆的逆解算
	float x_dot_dot_ref = (g * qsin(theta) - theta_dot_dot_ref * lp) / qcos(theta);
	
	//计算轮胎转速（原硬编码0.005f，改为使用实际deltaT）
	if(lastTime != 0)
	{
		omega_ref += 1.0f / rw * x_dot_dot_ref * deltaT;
	}
	
	float gz = App_mpu6050_GetGz() * 0.017453f;
	float omega_diff = PID_Compute(&pid_turn,gz);
	
	//设置轮胎转速
	//原代码：左右轮设置相同转速omega_ref，未应用转向PID输出omega_diff，导致无法左右转弯
	//App_Motor_SetOmega_L(omega_ref);
	//App_Motor_SetOmega_R(omega_ref);
	//修复：左右轮差速控制，叠加转向PID输出omega_diff实现转弯（左轮加、右轮减）
	//若转向方向相反，将下方 + / - 互换即可
	App_Motor_SetOmega_L(omega_ref + omega_diff);
	App_Motor_SetOmega_R(omega_ref - omega_diff);
	
	lastTime = now;
}

//复位
void App_Control_Reset(void)
{
	lastTime = 0;
	omega_ref = 0;
	
	PID_Reset(&pid_velocity);
	PID_Reset(&pid_theta);
	PID_Reset(&pid_theta_dot);
}

//改变平衡车的速度
void App_Control_SetMoveSpeed(float MoveSpeed)
{
	PID_ChangeSP(&pid_velocity, MoveSpeed);
}
	
void App_Control_SetTurnSpeed(float TurnSpeed)
{
	PID_ChangeSP(&pid_turn,TurnSpeed);
}
