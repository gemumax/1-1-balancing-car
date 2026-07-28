#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "app_bat_test.h"
#include "app_bat.h"                      
#include "app_button.h"
#include "app_pwm.h"
#include "pwm_test.h"
#include "encoder_test.h"
#include "mpu6050_test.h"
#include "qmath_test.h"
#include "app_encoder.h"
#include "app_motor.h"
#include "app_usart.h"
#include "task.h"
#include "app_mpu6050.h"
#include "app_control.h"
#include "app_rc.h"

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);   
	//Qmath_Test();
	//MPU6050_EularAngleTest();
	//MPU6050_Tste();
	//encoder_T_Method_Test();
	//encoder_M_Method_Test();
	//encoder_Test();	
	//Bat_Test();
	//PWM_Test();
	App_Button_Init();
	App_Encoder_Init();
	App_PWM_Init();
    App_Bat_Init();
	App_Motor_Init();
	App_USART2_Init();
	App_mpu6050_Init();
	App_Control_Init();
	App_RC_Init();
    
    while(1)
    {
		
		App_Bat_proc(); 
        App_Button_Proc();		
		App_Motor_Proc();
		App_mpu6050_Proc();
		App_Control_Proc();
        App_RC_Proc();                                                                                                                 
    }
}
