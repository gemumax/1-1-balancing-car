#include "stm32f10x.h"                  // Device header
#include "pid.h"
#include "delay.h"

void PID_Init(PID_TypeDef *PID, float Kp, float Ki, float Kd)
{
	PID->Kp = Kp;
	PID->Ki = Ki;
	PID->Kd = Kd;
	PID->SP = 0.0f;
	PID->t_k_1 = 0;
	PID->err_k_1 = 0.0f;
	PID->err_int_k_1 = 0.0f;
	PID->UpperLimit = +3.4e+38f;
	PID->LowerLimit = -3.4e+38f;
}

//改变设置值SP
void PID_ChangeSP(PID_TypeDef *PID, float SP)
{
	PID->SP = SP;
}

//执行一次PID运算
float PID_Compute(PID_TypeDef *PID, float FB)
{
	float err = PID->SP - FB;
	
	uint64_t t_k = GetUs();//获取当前时间单位：s
	float detaT = (t_k - PID->t_k_1) * 1.0e-6f;
	
	float err_dev =0.0f;
	float err_int =0.0f;
	if(PID->t_k_1 != 0)
	{
		err_dev = (err - PID->err_k_1) / detaT;//微分
		err_int = PID->err_int_k_1 + (err + PID->err_k_1) * detaT * 0.5f;//积分，上底加下底的和乘高除以2
	}
	
	float COp = PID->Kp * err;
	float COi = PID->Ki * err_int;
	float COd = PID->Kd * err_dev;
	float CO = COp + COi + COd;
	
	PID->t_k_1 = t_k;
	PID->err_k_1 = err;
	PID->err_int_k_1 = err_int;
	
	//输出限幅
	if(CO > PID->UpperLimit) CO = PID->UpperLimit;
	if(CO < PID->LowerLimit) CO = PID->LowerLimit;
	
	//积分限幅
	if(PID->err_int_k_1 > PID->UpperLimit) PID->err_int_k_1 = PID->UpperLimit;
	if(PID->err_int_k_1 < PID->LowerLimit) PID->err_int_k_1 = PID->LowerLimit;
	
	return CO;
}

//设置PID门限
//upper：上限
//lower：下限
void PID_LimitConfig(PID_TypeDef *PID, float Upper, float Lower)
{
	PID->UpperLimit = Upper;
	PID->LowerLimit = Lower;
}

//对pid进行复位
void PID_Reset(PID_TypeDef *PID)
{
	PID->t_k_1 = 0;
	PID->err_k_1 = 0.0f;
	PID->err_int_k_1 = 0.0f;
}
